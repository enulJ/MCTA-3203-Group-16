// === Pin definitions for L298P Shield ===

// Motor A Pins (Motor 1)
const int ENA = 3;   // PWM Speed A (D3)
const int IN1 = 12;  // Direction A 1 (D12)
const int IN2 = 13;  // Direction A 2 (D13)

// Motor B Pins (Motor 2)
const int ENB = 5;   // PWM Speed B (D5)
const int IN3 = 10;  // Direction B 1 (D10)
const int IN4 = 11;  // Direction B 2 (D11)

// Define speeds for demonstration (0-255)
const int speedA = 220; // Fast
const int speedB = 120; // Medium

void setup() {
  // Initialize Motor A Pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  // Initialize Motor B Pins
  pinMode(ENB, OUTPUT); 
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Dual Motor Control Demo Started.");
}

void loop() {
  // === 1. Motor A Forward (Fast) and Motor B Reverse (Medium) ===
  Serial.println("--- State 1: A Forward (220) | B Reverse (120) ---");
  
  // Motor A: Forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speedA);
  
  // Motor B: Reverse
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, speedB);
  
  delay(3000); // Run for 3 seconds

  // === 2. Soft Stop (Coast to Stop) ===
  Serial.println("--- State 2: Soft Stop ---");
  analogWrite(ENA, 0); // Stop Motor A
  analogWrite(ENB, 0); // Stop Motor B
  delay(1000); // Pause for 1 second
  
  // === 3. Motor A Reverse (Fast) and Motor B Forward (Medium) ===
  Serial.println("--- State 3: A Reverse (220) | B Forward (120) ---");
  
  // Motor A: Reverse
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speedA);
  
  // Motor B: Forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, speedB);
  
  delay(3000); // Run for 3 seconds
  
  // === 4. Dynamic Braking (Quick Stop) ===
  Serial.println("--- State 4: Dynamic Braking ---");
  // Set both direction pins HIGH to short the motor leads while enable is active
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH); // Brake Motor A
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH); // Brake Motor B
  analogWrite(ENA, 255);   // Ensure ENA/ENB are active for braking
  analogWrite(ENB, 255);
  delay(2000); // Brake for 2 seconds
}