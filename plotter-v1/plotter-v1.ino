// --- ULN2003 Motor Driver ---
const int ULN2003_IN1 = 2;  // Connect to ESP32 GPIO 2 (or any suitable GPIO)
const int ULN2003_IN2 = 4;  // Connect to ESP32 GPIO 4
const int ULN2003_IN3 = 16; // Connect to ESP32 GPIO 16
const int ULN2003_IN4 = 17; // Connect to ESP32 GPIO 17

// --- L293D Motor Driver ---
// ENABLE pins (EN1,2 and EN3,4) must be HIGH for the H-bridges to operate.
const int L293D_IN1 = 18; // Connect to L293D IN1
const int L293D_IN2 = 19; // Connect to L293D IN2
const int L293D_IN3 = 21; // Connect to L293D IN3
const int L293D_IN4 = 22; // Connect to L293D IN4
const int L293D_ENABLE12 = 23; // Connect to L293D EN1,2 (keep HIGH for continuous operation)
const int L293D_ENABLE34 = 5;  // Connect to L293D EN3,4 (keep HIGH for continuous operation)

// --- Motor Parameters ---
// These default values can be overridden by serial input.
// ULN2003 Motor (e.g., X-axis)
int ulnStepsPerRevolution = 2048;          // Common for 28BYJ-48 in full step mode
long ulnStepDelayMicroseconds = 1000;      // Delay between steps for ULN2003 motor
long ulnStepsRemaining = 0;                // Steps remaining for the current movement
bool ulnMovingForward = true;              // Direction for current movement
int currentUlnStep = 0;                    // Current step in the 4-step sequence
unsigned long lastUlnStepTime = 0;         // Timestamp of the last step for ULN2003 motor

// L293D Motor (e.g., Y-axis)
int l293dStepsPerRevolution = 2048;         // Default for the second motor
long l293dStepDelayMicroseconds = 1000;     // Delay between steps for L293D motor
long l293dStepsRemaining = 0;               // Steps remaining for the current movement
bool l293dMovingForward = true;             // Direction for current movement
int currentL293dStep = 0;                   // Current step in the 4-step sequence
unsigned long lastL293dStepTime = 0;        // Timestamp of the last step for L293D motor

/**
 * @brief Steps the motor connected to the ULN2003 driver.
 * This function uses the full-step sequence for a unipolar stepper.
 * @param step The current step in the 4-step sequence (0-3).
 * @param direction True for clockwise, false for counter-clockwise.
 */
void stepMotorULN2003(int step, bool direction) {
  // Define the stepping sequence for full-step mode (common for ULN2003)
  // This sequence cycles through the four coils
  if (direction) { // Clockwise (or positive direction for an axis)
    switch (step) {
      case 0: // 1000
        digitalWrite(ULN2003_IN1, HIGH);
        digitalWrite(ULN2003_IN2, LOW);
        digitalWrite(ULN2003_IN3, LOW);
        digitalWrite(ULN2003_IN4, LOW);
        break;
      case 1: // 0100
        digitalWrite(ULN2003_IN1, LOW);
        digitalWrite(ULN2003_IN2, HIGH);
        digitalWrite(ULN2003_IN3, LOW);
        digitalWrite(ULN2003_IN4, LOW);
        break;
      case 2: // 0010
        digitalWrite(ULN2003_IN1, LOW);
        digitalWrite(ULN2003_IN2, LOW);
        digitalWrite(ULN2003_IN3, HIGH);
        digitalWrite(ULN2003_IN4, LOW);
        break;
      case 3: // 0001
        digitalWrite(ULN2003_IN1, LOW);
        digitalWrite(ULN2003_IN2, LOW);
        digitalWrite(ULN2003_IN3, LOW);
        digitalWrite(ULN2003_IN4, HIGH);
        break;
    }
  } else { // Counter-clockwise (or negative direction for an axis)
    switch (step) {
      case 0: // 0001
        digitalWrite(ULN2003_IN1, LOW);
        digitalWrite(ULN2003_IN2, LOW);
        digitalWrite(ULN2003_IN3, LOW);
        digitalWrite(ULN2003_IN4, HIGH);
        break;
      case 1: // 0010
        digitalWrite(ULN2003_IN1, LOW);
        digitalWrite(ULN2003_IN2, LOW);
        digitalWrite(ULN2003_IN3, HIGH);
        digitalWrite(ULN2003_IN4, LOW);
        break;
      case 2: // 0100
        digitalWrite(ULN2003_IN1, LOW);
        digitalWrite(ULN2003_IN2, HIGH);
        digitalWrite(ULN2003_IN3, LOW);
        digitalWrite(ULN2003_IN4, LOW);
        break;
      case 3: // 1000
        digitalWrite(ULN2003_IN1, HIGH);
        digitalWrite(ULN2003_IN2, LOW);
        digitalWrite(ULN2003_IN3, LOW);
        digitalWrite(ULN2003_IN4, LOW);
        break;
    }
  }
}

/**
 * @brief Steps the motor connected to the L293D driver.
 * This function uses the full-step sequence for a unipolar stepper
 * where each coil is switched to ground via the L293D's H-bridge outputs.
 * (Assumes motor common is connected to VCC, and L293D inputs control low-side switches)
 * @param step The current step in the 4-step sequence (0-3).
 * @param direction True for clockwise, false for counter-clockwise.
 */
void stepMotorL293D(int step, bool direction) {
  // Full-step sequence for L293D driving a unipolar stepper (common to VCC)
  // L293D inputs will mirror the logic of ULN2003 inputs (setting one coil HIGH, others LOW)
  // The L293D's internal H-bridge will then connect the respective output to ground or VCC.
  // For unipolar, we typically just need to switch one output LOW (to ground) to energize a coil.
  // This assumes the motor's common wire is tied to the positive supply.
  if (direction) { // Clockwise (or positive direction for an axis)
    switch (step) {
      case 0: // Coil A HIGH (L293D IN1 HIGH, others LOW)
        digitalWrite(L293D_IN1, HIGH);
        digitalWrite(L293D_IN2, LOW);
        digitalWrite(L293D_IN3, LOW);
        digitalWrite(L293D_IN4, LOW);
        break;
      case 1: // Coil B HIGH
        digitalWrite(L293D_IN1, LOW);
        digitalWrite(L293D_IN2, HIGH);
        digitalWrite(L293D_IN3, LOW);
        digitalWrite(L293D_IN4, LOW);
        break;
      case 2: // Coil C HIGH
        digitalWrite(L293D_IN1, LOW);
        digitalWrite(L293D_IN2, LOW);
        digitalWrite(L293D_IN3, HIGH);
        digitalWrite(L293D_IN4, LOW);
        break;
      case 3: // Coil D HIGH
        digitalWrite(L293D_IN1, LOW);
        digitalWrite(L293D_IN2, LOW);
        digitalWrite(L293D_IN3, LOW);
        digitalWrite(L293D_IN4, HIGH);
        break;
    }
  } else { // Counter-clockwise (or negative direction for an axis)
    switch (step) {
      case 0: // Coil D HIGH
        digitalWrite(L293D_IN1, LOW);
        digitalWrite(L293D_IN2, LOW);
        digitalWrite(L293D_IN3, LOW);
        digitalWrite(L293D_IN4, HIGH);
        break;
      case 1: // Coil C HIGH
        digitalWrite(L293D_IN1, LOW);
        digitalWrite(L293D_IN2, LOW);
        digitalWrite(L293D_IN3, HIGH);
        digitalWrite(L293D_IN4, LOW);
        break;
      case 2: // Coil B HIGH
        digitalWrite(L293D_IN1, LOW);
        digitalWrite(L293D_IN2, HIGH);
        digitalWrite(L293D_IN3, LOW);
        digitalWrite(L293D_IN4, LOW);
        break;
      case 3: // Coil A HIGH
        digitalWrite(L293D_IN1, HIGH);
        digitalWrite(L293D_IN2, LOW);
        digitalWrite(L293D_IN3, LOW);
        digitalWrite(L293D_IN4, LOW);
        break;
    }
  }
}

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("ESP32 X-Y Plotter Control (ULN2003 & L293D)");
  Serial.println("Commands:");
  Serial.println("  SET_DELAY <ULN_Delay_us>,<L293D_Delay_us> (e.g., SET_DELAY 1000,1200)");
  Serial.println("  MOVE <ULN_Steps>,<L293D_Steps> (e.g., MOVE 200,-100)");
  Serial.println("    ULN_Steps: Positive for forward, negative for backward (X-axis)");
  Serial.println("    L293D_Steps: Positive for forward, negative for backward (Y-axis)");


  // Set ULN2003 motor driver pins as outputs
  pinMode(ULN2003_IN1, OUTPUT);
  pinMode(ULN2003_IN2, OUTPUT);
  pinMode(ULN2003_IN3, OUTPUT);
  pinMode(ULN2003_IN4, OUTPUT);

  // Set L293D motor driver pins as outputs
  pinMode(L293D_IN1, OUTPUT);
  pinMode(L293D_IN2, OUTPUT);
  pinMode(L293D_IN3, OUTPUT);
  pinMode(L293D_IN4, OUTPUT);
  pinMode(L293D_ENABLE12, OUTPUT);
  pinMode(L293D_ENABLE34, OUTPUT);

  // Ensure all motor pins are low initially to stop the motors
  digitalWrite(ULN2003_IN1, LOW);
  digitalWrite(ULN2003_IN2, LOW);
  digitalWrite(ULN2003_IN3, LOW);
  digitalWrite(ULN2003_IN4, LOW);

  digitalWrite(L293D_IN1, LOW);
  digitalWrite(L293D_IN2, LOW);
  digitalWrite(L293D_IN3, LOW);
  digitalWrite(L293D_IN4, LOW);

  // Enable L293D H-bridges (keep HIGH for operation)
  digitalWrite(L293D_ENABLE12, HIGH);
  digitalWrite(L293D_ENABLE34, HIGH);
}

void loop() {
  // Check for serial input commands
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n'); // Read until newline character
    input.trim(); // Remove leading/trailing whitespace

    Serial.print("Received: ");
    Serial.println(input);

    if (input.startsWith("SET_DELAY ")) {
      // Parse the input string for delays: "SET_DELAY <ULN_Delay_us>,<L293D_Delay_us>"
      String params = input.substring(10); // "SET_DELAY " has 10 characters
      int commaIndex = params.indexOf(',');
      if (commaIndex != -1) {
        long ulnD = params.substring(0, commaIndex).toInt();
        long l293dD = params.substring(commaIndex + 1).toInt();

        if (ulnD > 0 && l293dD > 0) {
          ulnStepDelayMicroseconds = ulnD;
          l293dStepDelayMicroseconds = l293dD;
          Serial.print("Delays updated: ULN2003 = ");
          Serial.print(ulnStepDelayMicroseconds);
          Serial.print(" us, L293D = ");
          Serial.print(l293dStepDelayMicroseconds);
          Serial.println(" us.");
        } else {
          Serial.println("Invalid delay values. Please enter positive integers for delays.");
          Serial.println("Usage: SET_DELAY <ULN_Delay_us>,<L293D_Delay_us> (e.g., SET_DELAY 1000,1200)");
        }
      } else {
        Serial.println("Invalid SET_DELAY format. Usage: SET_DELAY <ULN_Delay_us>,<L293D_Delay_us>");
      }
    } else if (input.startsWith("MOVE ")) {
      // Parse the input string for steps: "MOVE <ULN_Steps>,<L293D_Steps>"
      String params = input.substring(5); // "MOVE " has 5 characters
      int commaIndex = params.indexOf(',');
      if (commaIndex != -1) {
        long ulnS = params.substring(0, commaIndex).toInt();
        long l293dS = params.substring(commaIndex + 1).toInt();

        ulnStepsRemaining = abs(ulnS);      // Store absolute steps
        ulnMovingForward = (ulnS >= 0);     // Determine direction

        l293dStepsRemaining = abs(l293dS);  // Store absolute steps
        l293dMovingForward = (l293dS >= 0); // Determine direction

        Serial.print("Initiating movement: ULN2003 by ");
        Serial.print(ulnS);
        Serial.print(" steps, L293D by ");
        Serial.print(l293dS);
        Serial.println(" steps.");
      } else {
        Serial.println("Invalid MOVE format. Usage: MOVE <ULN_Steps>,<L293D_Steps> (e.g., MOVE 200,-100)");
      }
    } else {
      Serial.println("Unknown command. Please use SET_DELAY or MOVE.");
    }
  }

  // --- Non-blocking motor movement logic ---
  // Step ULN2003 motor if steps remain and enough time has passed
  if (ulnStepsRemaining > 0 && (micros() - lastUlnStepTime >= (unsigned long)ulnStepDelayMicroseconds)) {
    stepMotorULN2003(currentUlnStep % 4, ulnMovingForward);
    currentUlnStep = (currentUlnStep + (ulnMovingForward ? 1 : -1) + 4) % 4; // Update step, wrap around 0-3
    ulnStepsRemaining--; // Decrement steps remaining
    lastUlnStepTime = micros(); // Update last step time
  }

  // Step L293D motor if steps remain and enough time has passed
  if (l293dStepsRemaining > 0 && (micros() - lastL293dStepTime >= (unsigned long)l293dStepDelayMicroseconds)) {
    stepMotorL293D(currentL293dStep % 4, l293dMovingForward);
    currentL293dStep = (currentL293dStep + (l293dMovingForward ? 1 : -1) + 4) % 4; // Update step, wrap around 0-3
    l293dStepsRemaining--; // Decrement steps remaining
    lastL293dStepTime = micros(); // Update last step time
  }

  // Optional: Print status when movement completes (or if idle)
  // This could be made more sophisticated with a "movement_active" flag
  if (ulnStepsRemaining == 0 && l293dStepsRemaining == 0 && (micros() - lastUlnStepTime > 1000000 || micros() - lastL293dStepTime > 1000000)) {
    // Only print if both motors are idle and a reasonable time has passed since last status print
    // To avoid spamming serial when idle
    static bool movementFinishedPrinted = false;
    if (!movementFinishedPrinted) {
        Serial.println("All movements complete. Waiting for new commands.");
        movementFinishedPrinted = true;
    }
  } else {
      static bool movementStarted = false;
      if(ulnStepsRemaining > 0 || l293dStepsRemaining > 0) {
          if(!movementStarted) {
              Serial.println("Motors are currently moving...");
              movementStarted = true;
          }
          movementFinishedPrinted = false; // Reset when movement starts
      } else {
          movementStarted = false; // Reset when movement stops
      }
  }
}
