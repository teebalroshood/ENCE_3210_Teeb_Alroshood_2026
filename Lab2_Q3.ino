// Question 3


#include <Arduino.h>                 // Include Arduino library

// -------- Button pins --------
const uint8_t BTN_SELECT = 2;        // Button on pin 2 used to select operation (interrupt)
const uint8_t BTN_EXEC   = 3;        // Button on pin 3 used to execute operation (interrupt)

// -------- LED pins --------
const uint8_t RED_LED   = 5;         // Red LED connected to pin 5
const uint8_t GREEN_LED = 4;         // Green LED connected to pin 4
const uint8_t BLUE_LED  = 7;         // Blue LED connected to pin 7

// -------- Operation control --------
volatile uint8_t op_mode = 1;        // Operation mode: 1=ADD, 2=SUB, 3=MUL
volatile bool exec_request = false;  // Flag to request calculation (set in ISR)

// -------- Debounce timing --------
volatile unsigned long last_sel_us = 0; // Last time select button was pressed
volatile unsigned long last_exe_us = 0; // Last time execute button was pressed
const unsigned long DEBOUNCE_US = 30000; // Debounce delay (30 ms)

// -------- Arrays --------
const int SIZE = 10;                 // Size of arrays
int arr1[SIZE]   = {1,2,3,4,5,6,7,8,9,10}; // First input array
int arr2[SIZE]   = {10,9,8,7,6,5,4,3,2,1}; // Second input array
int result[SIZE] = {0};              // Array to store results

// -------- Function to control LEDs --------
void setLEDs(bool r, bool g, bool b) {
  digitalWrite(RED_LED,   r ? HIGH : LOW);   // Turn red LED on/off
  digitalWrite(GREEN_LED, g ? HIGH : LOW);   // Turn green LED on/off
  digitalWrite(BLUE_LED,  b ? HIGH : LOW);   // Turn blue LED on/off
}

// -------- ISR for select button --------
// Cycles through operations: ADD → SUB → MUL
void ISR_selectOp() {
  unsigned long now = micros();               // Get current time
  if (now - last_sel_us > DEBOUNCE_US) {      // Check debounce
    op_mode++;                                // Move to next operation
    if (op_mode > 3) op_mode = 1;             // Wrap back to ADD
    last_sel_us = now;                        // Save press time
  }
}

// -------- ISR for execute button --------
// Requests execution of selected operation
void ISR_execute() {
  unsigned long now = micros();               // Get current time
  if (now - last_exe_us > DEBOUNCE_US) {      // Check debounce
    exec_request = true;                      // Signal execution request
    last_exe_us = now;                        // Save press time
  }
}

// -------- Setup function --------
void setup() {
  pinMode(BTN_SELECT, INPUT_PULLUP);          // Set select button as input
  pinMode(BTN_EXEC,   INPUT_PULLUP);          // Set execute button as input

  pinMode(RED_LED, OUTPUT);                   // Set red LED as output
  pinMode(GREEN_LED, OUTPUT);                 // Set green LED as output
  pinMode(BLUE_LED, OUTPUT);                  // Set blue LED as output

  // Start with ADD mode selected
  setLEDs(true, false, false);                // Red LED ON

  // Attach interrupts to buttons
  attachInterrupt(digitalPinToInterrupt(BTN_SELECT), ISR_selectOp, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_EXEC),   ISR_execute,  FALLING);
}

// -------- Main loop --------
void loop() {
  uint8_t mode;                               // Local copy of operation mode

  // Safely copy operation mode from ISR
  noInterrupts();
  mode = op_mode;
  interrupts();

  // Indicate selected operation using LEDs
  if (mode == 1) setLEDs(true,  false, false); // ADD → red LED
  if (mode == 2) setLEDs(false, true,  false); // SUB → green LED
  if (mode == 3) setLEDs(false, false, true ); // MUL → blue LED

  bool do_exec = false;                       // Flag to start calculation

  // Safely check execution request
  noInterrupts();
  if (exec_request) {
    exec_request = false;                     // Clear request
    do_exec = true;                           // Allow execution
  }
  interrupts();

  // Perform calculation if requested
  if (do_exec) {
    setLEDs(false, false, false);             // Turn all LEDs OFF during calculation

    // Apply selected operation to all array elements
    for (int i = 0; i < SIZE; i++) {
      if (mode == 1) result[i] = arr1[i] + arr2[i];      // Addition
      else if (mode == 2) result[i] = arr1[i] - arr2[i]; // Subtraction
      else result[i] = arr1[i] * arr2[i];                // Multiplication
    }

    setLEDs(true, true, true);                // Turn all LEDs ON when done
  }

  delay(20);                                  // Small delay for stability
}
