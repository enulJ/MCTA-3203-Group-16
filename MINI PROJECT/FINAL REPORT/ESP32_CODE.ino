#define BLYNK_TEMPLATE_ID "TMPL68DH22hUq"
#define BLYNK_TEMPLATE_NAME "MAKE ULTRASONIC"
#define BLYNK_AUTH_TOKEN "Kd8De09QUdeABAgYkBSIyjiaDrcA8z6G"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Faiz";
char pass[] = "faizhensem";

#define TRIG_PIN 32
#define ECHO_PIN 33

// ===== SETTINGS =====
#define DETECT_DISTANCE 80   // cm

BlynkTimer timer;

int objectCount = 0;
bool objectDetected = false;

// ===== ULTRASONIC READ =====
float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;

  return duration * 0.0343 / 2;
}

// ===== CHECK & COUNT =====
void checkSensor() {
  float distance = readDistanceCM();

  if (distance > 0 && distance < DETECT_DISTANCE) {
    if (!objectDetected) {
      objectDetected = true;
      objectCount++;

      Serial.print("Object detected! Count = ");
      Serial.println(objectCount);

      Blynk.virtualWrite(V0, objectCount);
    }
  } else {
    objectDetected = false;
  }

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  Blynk.virtualWrite(V1, distance);
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Grove Port 2 Ultrasonic Counter Started");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(300L, checkSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}