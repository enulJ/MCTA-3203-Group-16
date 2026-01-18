#include "HX711.h"
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
SoftwareSerial espSerial(1, 0); // Arduino pins RX, TX


// ===== PINS =====
const int DOUT_PIN = 2;
const int CLK_PIN  = 3;

const int START_BTN = 5;
const int STOP_BTN  = 4;

const int GREEN_LED = 6;
const int RED_LED   = 7;

// ===== MOTOR + ENCODER =====
const int IN1 = 9;
const int IN2 = 8;
const int ENA = 10;

const int encoderA = 12;
const int encoderB = 13;

const int BUZZER = 11;

unsigned long motorTimer = 0;
int motorPhase = 0; // 0 = CW, 1 = STOP, 2 = CCW, 3 = STOP


// ===== OBJECTS =====
LiquidCrystal_I2C lcd(0x27,16,2);
HX711 scale;

// ===== VARIABLES =====
float calibration_factor = 998;
float weight = 0.0;

// ===== ENCODER =====
volatile long encoderCount = 0;
unsigned long lastRPMTime = 0;
float rpm = 0;
const int PPR = 6;   // SPG30 encoder pulses per rev

// ===== STATES =====
enum State {IDLE, WASHING, DRAINING, PAUSED};
State currentState = IDLE;
State prevState    = IDLE;

// ===== TIMERS =====
unsigned long timerStart = 0;
unsigned long remainingTime = 0;
const unsigned long WASH_TIME  = 15000;
const unsigned long DRAIN_TIME = 15000;

// ===== BUTTON EDGE =====
bool lastStartBtn = LOW;
bool lastStopBtn  = LOW;
bool stopRequest = false;

// ===== ENCODER ISR =====
void encoderISR() {
  if (digitalRead(encoderB) == HIGH)
    encoderCount++;
  else
    encoderCount--;
}

// ===== FUNCTION PROTOTYPES =====
void motorCW();
void motorCCW();
void motorStop();
void calculateRPM();
void displayWeight();
void buzzerBeep(int times, int duration);
void lockWeight();
unsigned long getRemainingTime();
unsigned long getTargetTime();
void runMotorSequence();

// ===== MOTOR + ENCODER FUNCTIONS =====
void motorCW(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 150);
}

void motorCCW(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 150);
}

void motorStop(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}

void setup() {
  Serial.begin(38400);
  espSerial.begin(9600);

  lcd.init();
  lcd.backlight();

  pinMode(START_BTN, INPUT); 
  pinMode(STOP_BTN, INPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderA), encoderISR, RISING);

  pinMode(BUZZER, OUTPUT);

  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(calibration_factor);

  lcd.print("Stabilizing...");
  delay(2000);
  scale.tare();

  lcd.clear();
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();

  motorStop();
}

// ===== LOOP =====
void loop() {

  bool startNow = digitalRead(START_BTN); 
  bool stopNow = digitalRead(STOP_BTN);

  espSerial.println("START_WASH");

  if(stopNow == HIGH && lastStopBtn == LOW){
    stopRequest = true;
  }

  bool startPressed = (startNow == HIGH && lastStartBtn == LOW);

  lastStartBtn = startNow;
  lastStopBtn  = stopNow;

  if(currentState == IDLE){
    displayWeight();
    motorStop();
  }

  if(stopRequest){
    if(currentState == WASHING || currentState == DRAINING){
      prevState = currentState;
      remainingTime = getRemainingTime();
      currentState = PAUSED;

      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      motorStop();
      buzzerBeep(1, 300);

      lcd.clear();
      lcd.print("     PAUSED   ");
    }
    else if(currentState == PAUSED){
      currentState = IDLE;
      lcd.clear();
      motorStop();
    }
    stopRequest = false;
  }

  if(startPressed){
    if(currentState == PAUSED){
      currentState = prevState;
      timerStart = millis();
      lcd.clear();
      buzzerBeep(1, 200);
    }
    else if(currentState == IDLE){
      lockWeight();
      remainingTime = 0;
      currentState = WASHING;
      timerStart = millis();
      lockWeight();
      buzzerBeep(1, 300);
    }
  }

  // ===== WASHING =====
  if(currentState == WASHING){
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    lcd.setCursor(0,0);
    lcd.print("     WASH     ");

    runMotorSequence();
    calculateRPM();

    if(millis() - timerStart >= getTargetTime()){
      currentState = DRAINING;
      timerStart = millis();
      motorStop();
      motorPhase = 0;
    }
  }

  // ===== DRAINING =====
  if(currentState == DRAINING){
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    lcd.setCursor(0,0);
    lcd.print("    DRAIN     ");

    runMotorSequence();
    calculateRPM();

    if(millis() - timerStart >= getTargetTime()){
      motorStop();
      lcd.clear();
      lcd.print("DONE");
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      buzzerBeep(3,300);
      delay(2000);
      currentState = IDLE;
      lcd.clear();
    }
  }

} // <-- This was missing! Close loop() properly

// ===== FUNCTIONS =====
void calculateRPM(){
  if(millis() - lastRPMTime >= 1000){
    rpm = abs((encoderCount / (float)PPR) * 60.0);
    encoderCount = 0;
    lastRPMTime = millis();

    Serial.print("RPM: ");
    Serial.println(rpm);
  }
}

void displayWeight()
{
  if(scale.is_ready()){
    weight = scale.get_units(5);
    lcd.setCursor(0,0);
    lcd.print("Weight: ");
    lcd.print(weight,0);
    lcd.print(" g   ");
  }
}

void buzzerBeep(int times, int duration)
{
  for(int i=0;i<times;i++){
    digitalWrite(BUZZER,HIGH);
    delay(duration);
    digitalWrite(BUZZER,LOW);
    delay(100);
  }
}

void lockWeight(){
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);

  lcd.clear();
  if(weight < 100){
    lcd.print("Water: 100ml");
    lcd.setCursor(0,1);
    lcd.print("Time: 1 hour");
  }
  else if(weight < 200){
    lcd.print("Water: 200ml");
    lcd.setCursor(0,1);
    lcd.print("Time: 2 hour");
  }
  else{
    lcd.print("Water: 300ml");
    lcd.setCursor(0,1);
    lcd.print("Time: 3 hour");
  }

  delay(3000);
  lcd.clear();
}

void runMotorSequence() {
  if (millis() - motorTimer >= 1000) {   // 5 seconds per phase
    motorTimer = millis();
    motorPhase = (motorPhase + 1) % 4;
  }

  if (motorPhase == 0) motorCCW();
  else if (motorPhase == 1) motorStop();
  else if (motorPhase == 2) motorCCW();
  else motorStop();
}

unsigned long getRemainingTime(){
  unsigned long elapsed = millis() - timerStart;
  unsigned long target  = getTargetTime();
  if(elapsed >= target) return 0;
  return target - elapsed;
}

unsigned long getTargetTime(){
  if(currentState == WASHING || prevState == WASHING)
    return remainingTime == 0 ? WASH_TIME : remainingTime;
  if(currentState == DRAINING || prevState == DRAINING)
    return remainingTime == 0 ? DRAIN_TIME : remainingTime;
  return 0;
}