
// Question 1 – Button Counter with Interrupts


#include <Arduino.h>              // Include Arduino core library

// Button pin definitions
const uint8_t BTN1_PIN = 2;       // Button 1 connected to pin 2 (INT0 interrupt)
const uint8_t BTN2_PIN = 3;       // Button 2 connected to pin 3 (INT1 interrupt)

//  LED pin definitions
const uint8_t RED_LED   = 5;      // Red LED connected to digital pin 5
const uint8_t GREEN_LED = 4;      // Green LED connected to digital pin 4
const uint8_t BLUE_LED  = 7;      // Blue LED connected to digital pin 7

//  Button press counters 
// volatile because they are modified inside ISR functions
volatile uint32_t btn1_count = 0; // Stores number of times button 1 is pressed
volatile uint32_t btn2_count = 0; // Stores number of times button 2 is pressed

//  Debounce variables 
// Used to prevent false multiple counts from one button press
volatile unsigned long last1_us = 0;   // Last interrupt time for button 1
volatile unsigned long last2_us = 0;   // Last interrupt time for button 2
const unsigned long DEBOUNCE_US = 30000; // 30 ms debounce delay

//  ISR for Button 1 
// Runs automatically when button 1 is pressed
void ISR_btn1() {
  unsigned long now = micros();          // Get current time in microseconds
  if (now - last1_us > DEBOUNCE_US) {    // Check if debounce time has passed
    btn1_count++;                        // Increment button 1 counter
    digitalWrite(RED_LED, !digitalRead(RED_LED)); // Toggle red LED state
    last1_us = now;                      // Save current time for debounce
  }
}

//  ISR for Button 2 
// Runs automatically when button 2 is pressed
void ISR_btn2() {
  unsigned long now = micros();          // Get current time in microseconds
  if (now - last2_us > DEBOUNCE_US) {    // Check if debounce time has passed
    btn2_count++;                        // Increment button 2 counter
    digitalWrite(GREEN_LED, !digitalRead(GREEN_LED)); // Toggle green LED state
    last2_us = now;                      // Save current time for debounce
  }
}

//  Setup function 
// Runs once when the Arduino starts
void setup() {
  Serial.begin(9600);                    // Initialize serial communication

  // Configure button pins as inputs with internal pull-up resistors
  pinMode(BTN1_PIN, INPUT_PULLUP);       // Button 1 input
  pinMode(BTN2_PIN, INPUT_PULLUP);       // Button 2 input

  // Configure LED pins as outputs
  pinMode(RED_LED, OUTPUT);              // Red LED output
  pinMode(GREEN_LED, OUTPUT);            // Green LED output
  pinMode(BLUE_LED, OUTPUT);             // Blue LED output

  // Turn all LEDs off initially
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);

  // Attach interrupts to buttons
  // ISR is triggered when the button is pressed (HIGH → LOW)
  attachInterrupt(digitalPinToInterrupt(BTN1_PIN), ISR_btn1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN2_PIN), ISR_btn2, FALLING);
}

//  Main loop 
// Runs repeatedly after setup()
void loop() {
  uint32_t c1, c2;                       // Local copies of button counters

  // Temporarily disable interrupts to safely read shared variables
  noInterrupts();
  c1 = btn1_count;                       // Copy button 1 count
  c2 = btn2_count;                       // Copy button 2 count
  interrupts();                          // Re-enable interrupts

  //  Blue LED logic 
  // Turn blue LED ON if both buttons have been pressed at least once
  if (c1 > 0 && c2 > 0) {
    digitalWrite(BLUE_LED, HIGH);
  } else {
    digitalWrite(BLUE_LED, LOW);
  }

  // Print button counts to Serial Monitor
  Serial.print("B1=");
  Serial.print(c1);
  Serial.print("  B2=");
  Serial.println(c2);

  delay(450);                            // Delay to slow down serial output
}
