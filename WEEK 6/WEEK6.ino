#include <SPI.h>
#include <Pixy.h>
#include <Servo.h>

Pixy pixy;
Servo myservo;

const int servoPin = 9;
const int buttonPin = 10;

int servoPos = 90;       // Start centered
int centerX = 160;       // Pixy frame center
int tolerance = 10;      // ± pixels considered centered
int step = 1;            // how much servo moves per loop

bool startCentering = false;  // Flag to start centering when button is pressed

void setup() {
  Serial.begin(9600);
  Serial.println("Pixy + Servo Auto Centering");

  pixy.init();

  myservo.attach(servoPin);
  myservo.write(servoPos); // start centered

  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  // Detect button press (edge detection)
  static bool lastButtonState = HIGH;
  bool buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && lastButtonState == HIGH) {
    // Button was just pressed → start centering
    startCentering = true;
    Serial.println("Button pressed: start centering");
  }
  lastButtonState = buttonState;

  if (startCentering) {
    // Read Pixy blocks
    uint16_t n = pixy.getBlocks();

    if (n > 0) {
      int x = pixy.blocks[0].x;
      Serial.print("Object X: ");
      Serial.println(x);

      int error = x - centerX;

      // Check if object is centered
      if (abs(error) <= tolerance) {
        Serial.println("Object centered. Servo holding position.");
        startCentering = false; // stop centering until next button press
        return; 
      }

      // Move servo step by step toward center
      if (error > 0) {
        servoPos += step;  // object right → move servo left
      } else {
        servoPos -= step;  // object left → move servo right
      }

      // Limit servo to 0–180
      if (servoPos < 0) servoPos = 0;
      if (servoPos > 180) servoPos = 180;

      myservo.write(servoPos);
      Serial.print("Servo moving to: ");
      Serial.println(servoPos);

    } else {
      Serial.println("No object detected.");
    }

    delay(20); // smooth movement
  }
}