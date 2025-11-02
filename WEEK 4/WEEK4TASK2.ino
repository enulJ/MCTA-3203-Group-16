#include <Wire.h>
#include <Servo.h>
#include "MPU6050.h"

MPU6050 mpu;
Servo gateServo;

// Pin configuration
const int redLED = 3;
const int greenLED = 4;
const int servoPin = 9;

// Authorized UID list
String authorizedUIDs[] = {"0013043326", "0008824390"};  // Change to your actual UIDs
int numAuthorized = 2;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  gateServo.attach(servoPin);
  gateServo.write(0);

  Serial.println("Arduino Ready...");
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection successfully.");
  } else {
    Serial.println("MPU6050 connected failed!");
  }
}

bool isAuthorized(String uid) {
  for (int i = 0; i < numAuthorized; i++) {
    if (uid == authorizedUIDs[i]) return true;
  }
  return false;
}

bool detectCircularMotion() {
  const unsigned long checkDuration = 4000; // 4 seconds for motion detection
  unsigned long startTime = millis();
  bool motionDetected = false;

  Serial.println("Perform circular motion now...");

  int16_t ax, ay, az;
  int motionCount = 0;

  while (millis() - startTime < checkDuration) {
    mpu.getAcceleration(&ax, &ay, &az);

    // Calculate total acceleration magnitude
    float magnitude = sqrt((float)(ax * ax + ay * ay + az * az));

    // Check for significant variation from rest (~16384)
    if (magnitude > 18000 || magnitude < 14000) {
      motionCount++;
    }

    delay(100);
  }

  if (motionCount > 10) {
    motionDetected = true;
  }

  return motionDetected;
}

void loop() {
  if (Serial.available()) {
    String uid = Serial.readStringUntil('\n');
    uid.trim();

    if (uid.length() == 0) return;

    Serial.print("Received UID: ");
    Serial.println(uid);

    if (isAuthorized(uid)) {
      Serial.println("UID Authorized.");

      bool motion = detectCircularMotion();

      if (motion) {
        // Access Granted
        Serial.println("A"); // Send back to Python
        digitalWrite(greenLED, HIGH);
        gateServo.write(90);
        delay(2000);
        gateServo.write(0);
        digitalWrite(greenLED, LOW);
      } else {
        // Access Denied (no circular motion)
        Serial.println("A"); // access granted
        Serial.flush();      // ensure data sent // Send back to Python
        digitalWrite(redLED, HIGH);
        delay(2000);
        digitalWrite(redLED, LOW);
      }

    } else {
      // Unauthorized UID
      Serial.println("D"); // access granted
      Serial.flush();      // ensure data sent
      digitalWrite(redLED, HIGH);
      delay(2000);
      digitalWrite(redLED, LOW);
    }
  }
}
