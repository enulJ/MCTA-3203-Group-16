#include <Pixy.h>

Pixy pixy; // Pixy v1 object

const int LED_PIN = 7;

void setup() {
  Serial.begin(9600);
  pixy.init();
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("Pixy v1 Ready");
}

void loop() {
  int32_t blocks = pixy.getBlocks();

  if (blocks > 0) {
    // Read the first detected block
    int sig = pixy.blocks[0].signature;
    int x   = pixy.blocks[0].x;
    int y   = pixy.blocks[0].y;
    int w   = pixy.blocks[0].width;
    int h   = pixy.blocks[0].height;

    // Print data for Python or Serial Monitor
    Serial.print("ID:");
    Serial.print(sig);
    Serial.print(",X:");
    Serial.print(x);
    Serial.print(",Y:");
    Serial.print(y);
    Serial.print(",W:");
    Serial.print(w);
    Serial.print(",H:");
    Serial.println(h);

    digitalWrite(LED_PIN, HIGH);  // turn on LED if any color found
  }
  else {
    digitalWrite(LED_PIN, LOW);   // no object detected
  }

  delay(50);
}
