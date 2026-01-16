#include <Arduino.h>                 // Include Arduino core functions

const uint8_t BTN_PIN = 2;           // Button connected to pin 2 (interrupt pin)
const uint8_t RED_LED = 5;           // Red LED connected to pin 5
const uint8_t GREEN_LED = 4;         // Green LED connected to pin 4
const uint8_t BLUE_LED = 7;          // Blue LED connected to pin 7

volatile bool toggle_run = false;    // Flag to start/stop LED sequence (changed in ISR)
volatile unsigned long last_us = 0;  // Stores last button press time for debounce
const unsigned long DEBOUNCE_US = 30000; // Debounce time = 30 ms

// Interrupt Service Routine for button press
void ISR_toggleSequence() {
  unsigned long now = micros();              // Gets the current time in microseconds
  if (now - last_us > DEBOUNCE_US) {          // Checks if debounce time passed
    toggle_run = !toggle_run;                 // Toggle sequence ON or OFF
    last_us = now;                            // Save time of this button press
  }
}

// Function to turn RGB LEDs ON or OFF
void setRGB(bool r, bool g, bool b) {
  digitalWrite(RED_LED,   r ? HIGH : LOW);    // Turn red LED on/off
  digitalWrite(GREEN_LED, g ? HIGH : LOW);    // Turn green LED on/off
  digitalWrite(BLUE_LED,  b ? HIGH : LOW);    // Turn blue LED on/off
}

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);             // Set button pin as input with pull-up

  pinMode(RED_LED, OUTPUT);                   // Set red LED pin as output
  pinMode(GREEN_LED, OUTPUT);                 // Set green LED pin as output
  pinMode(BLUE_LED, OUTPUT);                  // Set blue LED pin as output

  setRGB(false, false, false);                // Turn all LEDs OFF initially

  // Attach interrupt to button (trigger on button press)
  attachInterrupt(digitalPinToInterrupt(BTN_PIN),
                  ISR_toggleSequence,
                  FALLING);
}

void loop() {
  static uint8_t step = 0;                    // Keeps track of LED sequence step

  bool running;                               // Local copy of run flag

  noInterrupts();                             // Disable interrupts briefly
  running = toggle_run;                      // Copy ISR flag safely
  interrupts();                               // Re-enable interrupts

  if (!running) {                             // If sequence is OFF
    setRGB(false, false, false);              // Turn all LEDs OFF
    delay(20);                                // Small delay
    return;                                   // Exit loop early
  }

  // LED sequence: R → RG → RGB → GB → B → RB
  switch (step) {
    case 0: setRGB(true,  false, false); break; // Red
    case 1: setRGB(true,  true,  false); break; // Red + Green
    case 2: setRGB(true,  true,  true ); break; // Red + Green + Blue
    case 3: setRGB(false, true,  true ); break; // Green + Blue
    case 4: setRGB(false, false, true ); break; // Blue
    case 5: setRGB(true,  false, true ); break; // Red + Blue
  }

  step = (step + 1) % 6;                      // Move to next sequence step
  delay(250);                                 // Controls the speed of LED changes
}
