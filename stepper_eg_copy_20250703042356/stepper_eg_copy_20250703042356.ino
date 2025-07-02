// --- Pin Definitions ---
// These GPIOs should match your wiring to the DRV8825 stepper drivers.
// They correspond to the pins used in your FluidNC config.yaml for consistency.

// X-axis Stepper Pins
#define X_STEP_PIN 17 // Connects to STEP pin on X-axis DRV8825
#define X_DIR_PIN 16  // Connects to DIR pin on X-axis DRV8825

// Y-axis Stepper Pins
#define Y_STEP_PIN 19 // Connects to STEP pin on Y-axis DRV8825
#define Y_DIR_PIN 18  // Connects to DIR pin on Y-axis DRV8825

// Shared Stepper Enable Pin (connects to EN/ENABLE pin on your DRV8825 board)
// This pin typically needs to be LOW to enable the stepper drivers.
#define STEPPER_ENABLE_PIN 13 // Connects to ENABLE pin for all DRV8825s

// --- Motor Control Settings ---
// Adjust this value to control motor speed.
// Smaller value = faster movement. Be careful not to make it too small
// as it can cause motors to skip steps or stall.
// Recommended starting point: 500 (microsecond delay per step)
// For faster movement, try 200, 100, or even lower, but test carefully.
#define STEP_DELAY_US 5000 // Delay in microseconds between step pulses

// --- Setup Function ---
void setup() {
  Serial.begin(115200); // Initialize serial communication at FluidNC's baud rate
  delay(100); // Short delay to allow serial to initialize

  Serial.println("\n--- Arduino Simple Stepper Test for ESP32 ---");
  Serial.println("Commands format: AXIS STEPS DIRECTION (e.g., X 1000 +)");
  Serial.println("  AXIS: X or Y");
  Serial.println("  STEPS: Positive integer number of steps to move");
  Serial.println("  DIRECTION: '+' for positive, '-' for negative");
  Serial.println("  Example: X 1600 +  (move X-axis 1600 steps in positive direction)");
  Serial.println("Type 'exit' to stop the script.");
  Serial.println("----------------------------------------------");

  // Set pin modes to OUTPUT
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(STEPPER_ENABLE_PIN, OUTPUT);

  // Enable stepper drivers (usually active LOW for DRV8825)
  digitalWrite(STEPPER_ENABLE_PIN, LOW);
  Serial.println("Stepper drivers enabled.");
}

// --- Helper Function to Perform Steps ---
void doSteps(int stepPin, int dirPin, int numSteps, char directionChar) {
  // Set direction
  if (directionChar == '+') {
    digitalWrite(dirPin, HIGH); // Adjust HIGH/LOW if your motor moves the wrong way
  } else {
    digitalWrite(dirPin, LOW);  // Adjust HIGH/LOW if your motor moves the wrong way
  }

  Serial.print("Moving ");
  Serial.print(numSteps);
  Serial.print(" steps on ");
  Serial.print(stepPin == X_STEP_PIN ? "X" : "Y"); // Identify axis for printing
  Serial.print(" axis in ");
  Serial.print(directionChar);
  Serial.println(" direction...");

  // Generate step pulses
  for (int i = 0; i < numSteps; i++) {
    digitalWrite(stepPin, HIGH); // Step pulse HIGH
    delayMicroseconds(STEP_DELAY_US);
    digitalWrite(stepPin, LOW);  // Step pulse LOW
    delayMicroseconds(STEP_DELAY_US);
  }
  Serial.println("Move complete.");
}

// --- Main Loop ---
void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); // Read entire line from serial
    command.trim(); // Remove leading/trailing whitespace (like newline)

    // Check for exit command
    if (command.equalsIgnoreCase("exit")) {
      Serial.println("Exiting script. Reset ESP32 to run again.");
      digitalWrite(STEPPER_ENABLE_PIN, HIGH); // Disable steppers on exit
      while(true); // Halt execution
    }

    // Parse the command string: AXIS STEPS DIRECTION
    // Example: "X 1000 +"
    int firstSpace = command.indexOf(' ');
    int secondSpace = command.indexOf(' ', firstSpace + 1);

    // Basic validation of command format
    if (firstSpace == -1 || secondSpace == -1) {
      Serial.println("Error: Invalid command format. Use: AXIS STEPS DIRECTION");
      return; // Exit function if format is bad
    }

    char axisChar = command.charAt(0); // First character is the axis
    String stepsStr = command.substring(firstSpace + 1, secondSpace); // Extract steps string
    char directionChar = command.charAt(secondSpace + 1); // Get direction character

    int numSteps = stepsStr.toInt(); // Convert steps string to integer

    // Validate parsed values
    if (numSteps <= 0) {
      Serial.println("Error: Number of steps must be a positive integer.");
      return;
    }
    if (directionChar != '+' && directionChar != '-') {
      Serial.println("Error: Direction must be '+' or '-'.");
      return;
    }

    // Call doSteps based on axis
    if (axisChar == 'X' || axisChar == 'x') {
      doSteps(X_STEP_PIN, X_DIR_PIN, numSteps, directionChar);
    } else if (axisChar == 'Y' || axisChar == 'y') {
      doSteps(Y_STEP_PIN, Y_DIR_PIN, numSteps, directionChar);
    } else {
      Serial.println("Error: Invalid axis. Please use 'X' or 'Y'.");
    }
  }
}