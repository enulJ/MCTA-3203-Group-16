// === Pin definitions ===
const int ENA = 9;    // PWM pin for motor speed
const int IN1 = 5;    // Direction pin 1
const int IN2 = 6;    // Direction pin 2

const int encoderA = 2; // Encoder channel A
const int encoderB = 3; // Encoder channel B

// === Variables ===
volatile long pulseCount = 0;
unsigned long lastTime = 0;
float rpm = 0;
int pwmValue = 0;
char direction = 'S';  // 'F' = forward, 'R' = reverse, 'S' = stop

// SPG30-20K: 11 PPR (motor shaft) × 20:1 gearbox = 220 PPR
const int pulsesPerRevolution = 220;

// === Encoder interrupt routine ===
void updateEncoder() {
  int MSB = digitalRead(encoderA);
  int LSB = digitalRead(encoderB);
  int encoded = (MSB << 1) | LSB;
  static int lastEncoded = 0;

  int sum = (lastEncoded << 2) | encoded;
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) pulseCount++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) pulseCount--;

  lastEncoded = encoded;
}

// === Setup ===
void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderA), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderB), updateEncoder, CHANGE);

  Serial.println("Commands:");
  Serial.println("F <value> → Forward (0–255)");
  Serial.println("R <value> → Reverse (0–255)");
  Serial.println("S → Stop");
  Serial.println("---------------------------");
  lastTime = millis();
}

// === Loop ===
void loop() {
  // === Serial Command Reading ===
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    if (cmd == 'F' || cmd == 'f') {
      pwmValue = Serial.parseInt();
      direction = 'F';
      Serial.print("Forward at PWM = ");
      Serial.println(pwmValue);
    } 
    else if (cmd == 'R' || cmd == 'r') {
      pwmValue = Serial.parseInt();
      direction = 'R';
      Serial.print("Reverse at PWM = ");
      Serial.println(pwmValue);
    } 
    else if (cmd == 'S' || cmd == 's') {
      direction = 'S';
      pwmValue = 0;
      Serial.println("Motor stopped");
    }
  }

  // === Motor Control ===
  if (direction == 'F') {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, pwmValue);
  } 
  else if (direction == 'R') {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, pwmValue);
  } 
  else {
    analogWrite(ENA, 0);
  }

  // === RPM Measurement ===
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {
    noInterrupts();
    long currentCount = pulseCount;
    pulseCount = 0;
    interrupts();

    rpm = (abs(currentCount) / (float)pulsesPerRevolution) * 60.0;

    Serial.print("Direction: ");
    Serial.print(direction);
    Serial.print(" | PWM: ");
    Serial.print(pwmValue);
    Serial.print(" | RPM: ");
    Serial.println(rpm);

    lastTime = currentTime;
  }
}
