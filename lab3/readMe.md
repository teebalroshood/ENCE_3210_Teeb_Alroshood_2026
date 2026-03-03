lab 3
# LAB 3
Interrupt routines, Timers, GPIO input/output, volatile variables

## Overview
This program runs 2 interrupt driven tasks in the background plus a foreground main loop task.

- Task 1 uses Timer1 at 1 Hz
  - Blinks the Green LED at 1 Hz
  - Increments a shared counter used as a "timer" for the main task
- Task 2 uses Timer2 and a software divider to run at 10 Hz
  - Reads a pushbutton input
  - If the button reads HIGH, turn ON the Red LED
  - Otherwise turn OFF the Red LED
- Main task (foreground)
  - Prints a message to Serial every 3 seconds
  - Uses the Task 1 counter as the time base

## Hardware and pinout (edit if your board is different)
This code is written for an Arduino Uno style ATmega328P board using hardware Timer1 and Timer2.

| Signal | Arduino Pin | Notes |
|---|---:|---|
| Green LED | 8 | Use a resistor (220 to 1k) in series |
| Red LED | 9 | Use a resistor (220 to 1k) in series |
| Button input | 2 | Uses internal pullup, connect button to GND |

If your board has different pins or LEDs, only change the defines below.

## How it works
- Timer1 runs in CTC mode and triggers an interrupt exactly once per second (1 Hz).
- Timer2 runs in CTC mode at 1 kHz. Inside its ISR we count 100 ticks to get 10 Hz, then read the button and update the Red LED.
- Shared variables updated inside ISRs are declared `volatile`.

## Code (copy and paste into your .ino)
```cpp
/*
  LAB 3
  Two background tasks using Timer1 (1 Hz) and Timer2 (10 Hz via divider),
  plus a foreground main task printing every 3 seconds.

  Board: ATmega328P (Arduino Uno or compatible)
*/

#include <Arduino.h>

// ---------------- Pin configuration ----------------
#define GREEN_LED_PIN 8
#define RED_LED_PIN   9
#define BUTTON_PIN    2   // Uses INPUT_PULLUP, pressed = LOW if wired to GND

// ---------------- Shared state (must be volatile) ----------------
volatile uint32_t task1_seconds = 0;    // Incremented once per second by Task 1
volatile bool green_led_state = false;  // Toggled by Task 1

// Timer2 runs at 1 kHz, we divide by 100 to get 10 Hz
volatile uint16_t t2_tick_count = 0;

// ---------------- Task 1 (Timer1 ISR at 1 Hz) ----------------
ISR(TIMER1_COMPA_vect)
{
  green_led_state = !green_led_state;
  digitalWrite(GREEN_LED_PIN, green_led_state ? HIGH : LOW);

  // Counter used by the main task as a "timer"
  task1_seconds++;
}

// ---------------- Task 2 (Timer2 ISR at 10 Hz using divider) ----------------
ISR(TIMER2_COMPA_vect)
{
  t2_tick_count++;

  // Every 100 ticks of 1 kHz = 10 Hz
  if (t2_tick_count >= 100)
  {
    t2_tick_count = 0;

    // Read button and control Red LED
    int button_val = digitalRead(BUTTON_PIN);

    // If using INPUT_PULLUP and button wired to GND:
    // pressed -> LOW, not pressed -> HIGH
    // The lab spec says: if input pin equals 1 (HIGH), turn ON the Red LED.
    if (button_val == HIGH)
      digitalWrite(RED_LED_PIN, HIGH);
    else
      digitalWrite(RED_LED_PIN, LOW);
  }
}

// ---------------- Timer setup functions ----------------
static void setupTimer1_1Hz()
{
  // Timer1 in CTC mode, prescaler 1024
  // 16 MHz / 1024 = 15625 counts/sec
  // OCR1A = 15624 gives 1 Hz compare match
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 15624;
  TCCR1B |= (1 << WGM12);                 // CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10);    // prescaler 1024
  TIMSK1 |= (1 << OCIE1A);                // enable compare interrupt
  sei();
}

static void setupTimer2_1kHz()
{
  // Timer2 in CTC mode, prescaler 64
  // 16 MHz / 64 = 250000 ticks/sec
  // OCR2A = 249 gives 250000 / (249+1) = 1000 Hz
  cli();
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;

  OCR2A = 249;
  TCCR2A |= (1 << WGM21);                 // CTC mode
  TCCR2B |= (1 << CS22);                  // prescaler 64 (CS22=1, CS21=0, CS20=0)
  TIMSK2 |= (1 << OCIE2A);                // enable compare interrupt
  sei();
}

void setup()
{
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  while (!Serial) { }

  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);

  setupTimer1_1Hz();
  setupTimer2_1kHz();

  Serial.println("LAB 3 started");
}

void loop()
{
  // Foreground task prints every 3 seconds using the Task 1 counter
  static uint32_t last_print = 0;

  // Take an atomic snapshot of the shared counter
  uint32_t now_s;
  noInterrupts();
  now_s = task1_seconds;
  interrupts();

  if ((now_s - last_print) >= 3)
  {
    last_print = now_s;

    Serial.print("Main task message at t = ");
    Serial.print(now_s);
    Serial.println(" seconds");
  }

  // Optional small delay so loop is not max speed
  delay(5);
}
