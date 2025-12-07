#include "DHT.h"
#include <SoftwareSerial.h>

// ---------------------------
// HARDWARE CONFIG
// ---------------------------
#define DHTPIN 7        // DHT11 data pin
#define DHTTYPE DHT11
#define LEDPIN 8        // LED or FAN indicator

DHT dht(DHTPIN, DHTTYPE);

// HC-06 Bluetooth: D2 = RX, D3 = TX
SoftwareSerial bluetooth(2, 3);  // RX, TX

// ---------------------------
// SETUP
// ---------------------------
void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);

  dht.begin();
  pinMode(LEDPIN, OUTPUT);

  Serial.println("HC-06 Bluetooth + DHT11 System Ready");
}

// ---------------------------
// MAIN LOOP
// ---------------------------
void loop() {
  // ----- READ SENSOR -----
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  // Send sensor data via Bluetooth AND USB Serial Monitor
  if (!isnan(temp) && !isnan(hum)) {
    String tempStr = "TEMP:" + String(temp, 1);
    String humStr = "HUM:"  + String(hum, 1);
    
    // *** FIX IS HERE: Send separate lines to the USB Serial port ***
    Serial.println(tempStr);
    Serial.println(humStr);
    
    // Bluetooth output remains the same
    bluetooth.println(tempStr); 
    bluetooth.println(humStr);
    bluetooth.flush();
  }

  // ----- RECEIVE BLUETOOTH COMMANDS -----
  if (bluetooth.available()) {
    String cmd = bluetooth.readString();
    cmd.trim();
    
    String fanStatus;

    if (cmd.equalsIgnoreCase("FAN ON")) {
      digitalWrite(LEDPIN, HIGH);
      fanStatus = "FAN:ON";
    }
    else if (cmd.equalsIgnoreCase("FAN OFF")) {
      digitalWrite(LEDPIN, LOW);
      fanStatus = "FAN:OFF";
    }
    else {
      fanStatus = "FAN:UNKNOWN";
    }
    
    // Send fan status updates to both ports
    bluetooth.println(fanStatus);
    Serial.println(fanStatus);
  }

  delay(2000); // 2 seconds between readings
}
