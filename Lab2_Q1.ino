
#include <Arduino.h>

const uint8_t BTN1_PIN = 2;   // INT0
const uint8_t BTN2_PIN = 3;   // INT1

const uint8_t RED_LED   = 5;
const uint8_t GREEN_LED = 4;
const uint8_t BLUE_LED  = 7;  // new blue LED

volatile uint32_t btn1_count = 0;
volatile uint32_t btn2_count = 0;

// Simple debounce
volatile unsigned long last1_us = 0;
volatile unsigned long last2_us = 0;
const unsigned long DEBOUNCE_US = 30000; // 30 ms

void ISR_btn1() {
  unsigned long now = micros();
  if (now - last1_us > DEBOUNCE_US) {
    btn1_count++;
    digitalWrite(RED_LED, !digitalRead(RED_LED));
    last1_us = now;
  }
}

void ISR_btn2() {
  unsigned long now = micros();
  if (now - last2_us > DEBOUNCE_US) {
    btn2_count++;
    digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
    last2_us = now;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);

  attachInterrupt(digitalPinToInterrupt(BTN1_PIN), ISR_btn1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN2_PIN), ISR_btn2, FALLING);
}

void loop() {
  uint32_t c1, c2;

  noInterrupts();
  c1 = btn1_count;
  c2 = btn2_count;
  interrupts();

  // Blue LED logic example:
  if (c1 > 0 && c2 > 0) {
    digitalWrite(BLUE_LED, HIGH);
  } else {
    digitalWrite(BLUE_LED, LOW);
  }

  Serial.print("B1=");
  Serial.print(c1);
  Serial.print("  B2=");
  Serial.println(c2);

  delay(450);
}