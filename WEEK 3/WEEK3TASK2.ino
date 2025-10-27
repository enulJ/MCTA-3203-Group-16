/*
 * Enhanced Code: Potentiometer controls Servo AND an LED
 */

#include <Servo.h>

Servo myServo;

const int LED_PIN = 3;
const int POT_PIN = A0; // Arduino pin connected to the Potentiometer

void setup() {
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  // Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);
  
  // Attach the servo object to the servo pin (pin 9)
  myServo.attach(9); 
}

void loop() {
  // 1. Read the value of the potentiometer (value between 0 and 1023)
  int analogValue = analogRead(POT_PIN);

  // 2. Scale the analog value to the servo's angle range (0 to 180 degrees)
  int angle = map(analogValue, 0, 1023, 0, 180);

  // 3. Control the Servo
  myServo.write(angle); 

  // 4. Control the LED (The Enhancement)
  // Check if the angle is greater than 90 degrees (upper half of the range)
  if (angle > 90) {
    digitalWrite(LED_PIN, HIGH); // Turn LED ON
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  Serial.println(analogValue);
  
  delay(100);
}