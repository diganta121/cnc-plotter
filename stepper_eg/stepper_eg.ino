// Define the pins connected to the ULN2003 IN1, IN2, IN3, IN4
const int IN1 = 2; // Connect to ESP32 GPIO 2 (or any suitable GPIO)
const int IN2 = 4; // Connect to ESP32 GPIO 4
const int IN3 = 16; // Connect to ESP32 GPIO 16
const int IN4 = 17; // Connect to ESP32 GPIO 17

// Default values for steps per revolution and delay
// These can be overridden by serial input
int currentStepsPerRevolution = 2048; // Common for 28BYJ-48 in full step mode
int currentStepDelayMicroseconds = 1000; // 1000 microseconds = 1 millisecond

// Function to move the stepper motor one step in a specific direction
// mode: 0 for full step, 1 for half step (not implemented in this basic example)
// direction: true for clockwise, false for counter-clockwise
void stepMotor(int step, bool direction) {
  // Define the stepping sequence for full-step mode (common for ULN2003)
  // This sequence cycles through the four coils
  if (direction) { // Clockwise
    switch (step) {
      case 0: // 1000
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 1: // 0100
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 2: // 0010
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
      case 3: // 0001
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
    }
  } else { // Counter-clockwise
    switch (step) {
      case 0: // 0001
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
      case 1: // 0010
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
      case 2: // 0100
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 3: // 1000
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
    }
  }
}

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("ESP32 ULN2003 Stepper Motor Control");
  Serial.println("Enter 'steps,delay' (e.g., 2048,1000) to set motor parameters.");

  // Set the motor driver pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Ensure all pins are low initially to stop the motor
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {
  // Check for serial input
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n'); // Read until newline character
    input.trim(); // Remove leading/trailing whitespace

    Serial.print("Received: ");
    Serial.println(input);

    // Parse the input string (expecting "steps,delay")
    int commaIndex = input.indexOf(',');
    if (commaIndex != -1) {
      String stepsStr = input.substring(0, commaIndex);
      String delayStr = input.substring(commaIndex + 1);

      int newSteps = stepsStr.toInt();
      int newDelay = delayStr.toInt();

      if (newSteps > 0 && newDelay > 0) {
        currentStepsPerRevolution = newSteps;
        currentStepDelayMicroseconds = newDelay;
        Serial.print("Parameters updated: Steps = ");
        Serial.print(currentStepsPerRevolution);
        Serial.print(", Delay = ");
        Serial.print(currentStepDelayMicroseconds);
        Serial.println(" microseconds.");
      } else {
        Serial.println("Invalid numbers. Please enter positive integers for steps and delay.");
      }
    } else {
      Serial.println("Invalid format. Please use 'steps,delay' (e.g., 2048,1000).");
    }
  }

  // Rotate clockwise for one full revolution
  Serial.print("Rotating clockwise (Steps: ");
  Serial.print(currentStepsPerRevolution);
  Serial.print(", Delay: ");
  Serial.print(currentStepDelayMicroseconds);
  Serial.println(" us)...");
  for (int i = 0; i < currentStepsPerRevolution; i++) {
    stepMotor(i % 4, true); // Cycle through the 4-step sequence
    delayMicroseconds(currentStepDelayMicroseconds);
  }
  delay(1000); // Wait for 1 second

  // Rotate counter-clockwise for one full revolution
  Serial.print("Rotating counter-clockwise (Steps: ");
  Serial.print(currentStepsPerRevolution);
  Serial.print(", Delay: ");
  Serial.print(currentStepDelayMicroseconds);
  Serial.println(" us)...");
  for (int i = 0; i < currentStepsPerRevolution; i++) {
    stepMotor(i % 4, false); // Cycle through the 4-step sequence
    delayMicroseconds(currentStepDelayMicroseconds);
  }
  delay(1000); // Wait for 1 second
}
