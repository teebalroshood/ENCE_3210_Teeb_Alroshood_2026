/*******************************************************************************************

Q1 – The code below was given to you by a firmware engineering company that wants you to
improve it.

The main goal is to simulate in software a 14-bit SAR ADC and test the conversion for the
following analog voltage levels:

    Vin = 0.42 V, 0.83 V, 1.65 V, 2.752 V

with a reference voltage of:

    Vref = 3.0 V

--------------------------------------------------------------------------------------------
IMPROVEMENTS MADE:
- Fixed incorrect bit size (was 12 → now 14 bits)
- Corrected SAR algorithm logic (true MSB → LSB approximation)
- Removed incorrect threshold accumulation method
- Used proper DAC comparison equation
- Added readable output (binary + decimal + quantized voltage)
- Structured code for clarity and reuse
- Added detailed comments for understanding and GitHub readability
--------------------------------------------------------------------------------------------
*******************************************************************************************/

#include <stdio.h>
#include <stdint.h>

// Define ADC resolution (14-bit as required)
#define N_BITS 14

/*******************************************************************************************

FUNCTION: sar_convert



PURPOSE:
Performs a 14-bit Successive Approximation Register (SAR) ADC conversion.

HOW IT WORKS:
- Starts with MSB (Most Significant Bit)
- Tries setting each bit to 1
- Converts that trial code into an analog voltage using DAC formula
- Compares Vin with DAC output:
      if Vin >= Vdac → keep the bit = 1
      else → reset bit = 0
- Continues down to LSB

PARAMETERS:
- vin  → input analog voltage
- vref → reference voltage
- vq   → pointer to store quantized output voltage

RETURNS:
- Digital output code (14-bit integer)
- 
*******************************************************************************************/

static uint16_t sar_convert(double vin, double vref, double *vq)
{
    uint16_t code = 0;   // final digital output code

    // Loop from MSB → LSB
    for (int bit = N_BITS - 1; bit >= 0; bit--) {

        // Try setting current bit to 1
        uint16_t trial = code | (1 << bit);

        // Convert trial digital code → analog voltage (DAC equation)
        double v_dac = vref * ((double)trial / (1 << N_BITS));

        // Comparator decision
        if (vin >= v_dac) {
            code = trial;   // keep bit = 1
        }
        // else → bit remains 0 (do nothing)
    }

    // Compute final quantized voltage
    if (vq != NULL) {
        *vq = vref * ((double)code / (1 << N_BITS));
    }

    return code;
}

/*******************************************************************************************

FUNCTION: print_binary

PURPOSE:
Prints the 14-bit binary representation of the ADC output code.

WHY:
Because it is helpful for debugging and matches SAR hardware behavior
*******************************************************************************************/

static void print_binary(uint16_t code)
{
    for (int i = N_BITS - 1; i >= 0; i--) {
        if (code & (1 << i))
            printf("1");
        else
            printf("0");
    }
}

/*******************************************************************************************
MAIN FUNCTION

WHAT IT DOES:
- Defines test voltages
- Runs SAR conversion on each
- Prints:
    - Decimal code
    - Hex code
    - Binary representation
    - Quantized voltage
*******************************************************************************************/
int main(void)
{
    // Given reference voltage
    double Vref = 3.0;

    // Test input voltages
    double Vin_values[] = {0.42, 0.83, 1.65, 2.752};

    int num_tests = sizeof(Vin_values) / sizeof(Vin_values[0]);

    // Compute LSB size
    double LSB = Vref / (1 << N_BITS);

    printf("--------------------------------------------------\n");
    printf("14-bit SAR ADC Simulation\n");
    printf("Vref = %.2f V\n", Vref);
    printf("LSB  = %.12f V\n", LSB);
    printf("--------------------------------------------------\n\n");

    // Loop through all test voltages
    for (int i = 0; i < num_tests; i++) {

        double Vin = Vin_values[i];
        double Vq;   // quantized output voltage

        // Perform SAR conversion
        uint16_t code = sar_convert(Vin, Vref, &Vq);

        // Print results
        printf("Vin = %.6f V\n", Vin);

        printf("  Digital Code (Decimal): %u\n", code);
        printf("  Digital Code (Hex)    : 0x%04X\n", code);

        printf("  Binary               : ");
        print_binary(code);
        printf("\n");

        printf("  Quantized Voltage    : %.12f V\n\n", Vq);
    }

    return 0;
}
This project implements a 14-bit Successive Approximation Register (SAR) ADC in software.

The algorithm mimics real SAR ADC hardware by:
- Testing each bit from MSB to LSB
- Using a DAC model: Vdac = Vref * (code / 2^N)
- Comparing Vin with Vdac to decide each bit

Tested Inputs:
- 0.42 V
- 0.83 V
- 1.65 V
- 2.752 V

Reference Voltage:
- 3.0 V

Resolution:
- 14-bit (LSB = 3 / 16384 ≈ 0.0001831 V)

Outputs:
- Decimal code
- Hex code
- Binary representation
- Quantized voltage

This implementation corrects the original flawed approach and follows the proper SAR ADC algorithm.



Q2 – Solar Charge Controller (Hardware + Software Implementation)
Overview:
This project implements a solar charge controller using an Arduino Uno. Two adjustable potentiometers are used to simulate the solar panel voltage (Vsolar) and the battery voltage (Vbattery). The Arduino reads both voltages through its ADC pins, applies the decision rules from Q2, and reports the “power routing” and “charging” state through the Serial Monitor (and can optionally drive an LED as a simple indicator).
This lab focuses on demonstrating embedded system decision logic while interacting with real components on a breadboard.


System Requirements (Q2)
The controller follows these rules:
If Vsolar > 2.5 V, the microcontroller power is supplied from the solar panel
If Vsolar < 2.5 V, the battery supplies power to the microcontroller
If Vbattery < Vsolar, the system should connect the battery to the solar panel (charging condition)
Since a real switching IC is not available, the system must display how power would be routed
Hardware Used (Based on the Actual Circuit)
Arduino Uno
Breadboard and jumper wires
Two potentiometers (simulate solar and battery voltages)
One LED + series resistor (optional indicator)
Block Diagram:
  (simulated by Potentiometer 1)
                +--------------------------------------+
                |            Solar Source              |
                |             Vsolar                   |
                +-------------------+------------------+
                                    |
                                    |  ADC Sense (A0)
                                    v
                           +-------------------+
                           |  Microcontroller  |
                           |     Arduino Uno   |
                           |                   |
                           |  Decision Logic:  |
                           |  - Power source   |
                           |  - Charging state |
                           +----+----------+---+
                                |          |
                                |          |
                 Status Output   |          |   ADC Sense (A1)
              (Serial / LED)    |          v
                                |   +------------------------+
                                |   |  Battery Source         |
                                |   |  Vbattery               |
                                |   | (simulated by Pot 2)    |
                                |   +------------------------+
                                |
                                v
                       +-------------------+
                       | Display / Output  |
                       | Serial / LED      |
                       +-------------------+

Notes:
- “Switching Element” is not physically implemented.
- The Arduino simulates the switching decision and reports the routing state.


Circuit Explanation:

The breadboard uses two potentiometers as adjustable voltage dividers:

Each potentiometer has:

One end connected to 5 V
The other end connected to GND
The middle pin (wiper) produces a variable voltage between 0–5 V
Those wiper voltages are routed to the Arduino ADC pins:
A0 reads Vsolar (solar panel simulation)
A1 reads Vbattery (battery simulation)

- This matches Q2 because it gives the microcontroller two measurable voltages it can compare and use to decide:
whether “solar” or “battery” should power the system
whether the system should be in a “charging” condition (Vsolar > Vbattery)
Since we do not have a physical power multiplexer / analog switch IC, the “routing” is represented by printed messages (and optionally an LED).

Code for Q2
/*
Q2 – Solar Charge Controller (Arduino Uno)

This program reads:
- Vsolar  from analog pin A0 (potentiometer simulating solar panel voltage)
- Vbattery from analog pin A1 (potentiometer simulating battery voltage)

Decision rules:
1) If Vsolar > 2.5V -> Power Source = SOLAR
2) Else             -> Power Source = BATTERY
3) If Vbattery < Vsolar -> Charging = YES
   Else                 -> Charging = NO

Output:
- Serial Monitor text shows power routing and charging state
- Optional LED indicates charging state
*/

#include <Arduino.h>                 // Gives access to Arduino core functions

// --------------------------- Pin Definitions ---------------------------
#define SOLAR_PIN   A0               // Analog input for solar voltage (Vsolar)
#define BATTERY_PIN A1               // Analog input for battery voltage (Vbattery)

#define CHARGE_LED_PIN 7             // Optional: digital output LED pin (edit if needed)

// --------------------------- Constants ---------------------------
const float ADC_REF_V   = 5.0;       // Arduino Uno analog reference voltage (default = 5V)
const float ADC_COUNTS  = 1023.0;    // Arduino Uno is 10-bit ADC: 0..1023
const float SOLAR_THRESH = 2.5;      // Threshold from Q2

// --------------------------- Variables ---------------------------
float Vsolar   = 0.0;               // Measured solar voltage in volts
float Vbattery = 0.0;               // Measured battery voltage in volts

// --------------------------- Helper Function ---------------------------
/*
Reads an Arduino analog pin and converts the ADC reading to a voltage.
*/
static float readVoltage(uint8_t pin)
{
    int raw = analogRead(pin);                      // Read ADC value (0..1023)
    float v  = (raw / ADC_COUNTS) * ADC_REF_V;      // Convert ADC counts to voltage
    return v;                                       // Return voltage
}

void setup()
{
    Serial.begin(9600);                             // Start Serial Monitor output
    pinMode(CHARGE_LED_PIN, OUTPUT);                // Configure LED pin as output

    Serial.println("Q2 Solar Charge Controller بدء التشغيل");
    Serial.println("A0 = Vsolar, A1 = Vbattery");
    Serial.println("-----------------------------------");
}

void loop()
{
    // 1) Read both voltages from the potentiometers
    Vsolar   = readVoltage(SOLAR_PIN);              // Read solar simulation voltage
    Vbattery = readVoltage(BATTERY_PIN);            // Read battery simulation voltage

    // 2) Decide power source based on Vsolar threshold (Q2 b/c)
    bool solarPowersMCU = (Vsolar > SOLAR_THRESH);  // True if solar should power the MCU

    // 3) Decide charging condition (Q2 d)
    bool charging = (Vbattery < Vsolar);            // True if battery should charge from solar

    // 4) Output: Show routing (replaces physical switching element)
    Serial.print("Vsolar = ");
    Serial.print(Vsolar, 3);
    Serial.print(" V,  Vbattery = ");
    Serial.print(Vbattery, 3);
    Serial.println(" V");

    if (solarPowersMCU)
    {
        // Solar panel is “high enough” to be the power source
        Serial.println("Power Source: SOLAR -> MCU");

        if (charging)
        {
            // Battery voltage is below solar voltage, so charging condition is true
            Serial.println("Charging: YES (SOLAR -> BATTERY)");
        }
        else
        {
            // Battery is not below solar, so no charging
            Serial.println("Charging: NO");
        }
    }
    else
    {
        // Solar voltage is too low, so battery must power the system
        Serial.println("Power Source: BATTERY -> MCU");
        Serial.println("Charging: NO (solar below threshold)");
    }

    // 5) Optional LED: ON when charging, OFF otherwise
    digitalWrite(CHARGE_LED_PIN, charging ? HIGH : LOW);

    Serial.println("-----------------------------------");

    delay(500);                                     // Slow down output for readability

How the Code Corresponds to Q2
The potentiometers create adjustable analog voltages that represent Vsolar and Vbattery.
The ADC readings let the Arduino evaluate the conditions in Q2 using simple comparisons.
Because the switching element is not physically implemented, the Serial output (and optional LED) indicates:
which source would power the MCU
whether the battery would be charging








# Q3 – Improved Solar Charge Controller

## Overview

The original design in Q2 has a problem:
the system switches back to powering the microcontroller too early, which prevents the battery from fully charging.

This version fixes that issue by ensuring:

* The battery is allowed to **fully charge first**
* The system does **not switch back too early**

---

## Problem in Q2

In Q2:

* As soon as solar voltage is above 2.5 V → system switches to solar
* This interrupts charging before the battery is full

Result:

* Battery never reaches full charge

---

## Solution

We introduce a **charging priority condition**:

* If the battery is **not full**, keep charging
* Only switch back to powering the system once the battery is sufficiently charged

We define a **full battery threshold** (example):

```
Battery Full ≈ 4.0 V
```

---

## Updated Logic

1. If battery is NOT full → keep charging
2. If battery is full → allow normal switching
3. Solar still must be above 2.5 V to be used

---

## Code 

```cpp
#include <Arduino.h>              // Arduino functions

#define SOLAR_PIN A0             // Solar voltage input
#define BATTERY_PIN A1           // Battery voltage input

const float ADC_REF = 5.0;       // Reference voltage
const float ADC_MAX = 1023.0;    // 10-bit ADC

const float SOLAR_THRESHOLD = 2.5;   // From Q2
const float BATTERY_FULL = 4.0;      // New condition for Q3

float Vsolar;                    // Solar voltage
float Vbattery;                  // Battery voltage

// Function to convert ADC to voltage
float readVoltage(int pin)
{
    int raw = analogRead(pin);               // Read ADC value
    return (raw / ADC_MAX) * ADC_REF;        // Convert to voltage
}

void setup()
{
    Serial.begin(9600);                      // Start serial
}

void loop()
{
    // Read voltages
    Vsolar = readVoltage(SOLAR_PIN);
    Vbattery = readVoltage(BATTERY_PIN);

    Serial.print("Vsolar: ");
    Serial.println(Vsolar);

    Serial.print("Vbattery: ");
    Serial.println(Vbattery);

    // ---------------- IMPROVED LOGIC ----------------

    // Case 1: Battery NOT full → prioritize charging
    if (Vbattery < BATTERY_FULL)
    {
        if (Vsolar > SOLAR_THRESHOLD)
        {
            Serial.println("Charging Battery (priority mode)");
            Serial.println("Power Source: SOLAR → BATTERY");
        }
        else
        {
            Serial.println("No charging (solar too low)");
            Serial.println("Power Source: BATTERY → MCU");
        }
    }

    // Case 2: Battery IS full → normal behavior (same as Q2)
    else
    {
        if (Vsolar > SOLAR_THRESHOLD)
        {
            Serial.println("Battery Full");
            Serial.println("Power Source: SOLAR → MCU");
        }
        else
        {
            Serial.println("Battery Full");
            Serial.println("Power Source: BATTERY → MCU");
        }
    }

    Serial.println("----------------------");

    delay(1000);   // Delay for readability
}
```

##

---

## Summary

This improved design ensures that:

* The battery is properly charged before switching
* The system behaves more like a real solar charge controller
* The main flaw in Q2 is resolved
# Q4 – Fan Controller (Embedded System)

## Overview

This project implements a fan controller using an Arduino.
Since a real fan is not available, an LED is used to simulate the fan speed.

The system:

* Reads temperature using an analog input
* Averages readings for stability
* Controls fan speed using PWM
* Uses two buttons for control

---

## System Requirements

* Analog temperature sensor (simulated using potentiometer)
* 2 buttons:

  * Button 1 → ON/OFF
  * Button 2 → Sensitivity level
* PWM output (LED simulates fan)
* 100 samples over 5 seconds (average temperature)
* PWM frequency ≈ 250 Hz

---

## Block Diagram

```
        Temperature Sensor 
                      |
                      v
                +-------------+
                |  Arduino    |
                |             |
                | ADC Reading |
                | Averaging   |
                | Logic       |
                +------+------+ 
                       | 
        +--------------+--------------+
        |                             |
        v                             v
   Button 1                      Button 2
 (ON / OFF)                 (Sensitivity)

                       |
                       v
                PWM Output (LED)
                (Fan Speed)
```

---

## Code Flow Diagram

```
Start
  ↓
Read 100 samples (5 sec)
  ↓
Average temperature
  ↓
Check Button 1
  ↓
ON or OFF?
  ↓
If OFF → LED = 0
If ON:
    ↓
    Check sensitivity level
    ↓
    Adjust PWM based on temperature
    ↓
    Output PWM to LED
  ↓
Repeat
```

---

## Code (Fully Commented)

```cpp
#include <Arduino.h>

// ---------------- PINS ----------------
#define TEMP_PIN A0       // Temperature input (potentiometer)
#define LED_PIN  9        // PWM output (LED)
#define BTN1     2        // ON/OFF button
#define BTN2     3        // Sensitivity button

// ---------------- VARIABLES ----------------
bool fanOn = false;       // Fan state
int sensitivity = 1;      // 1 = low, 2 = medium, 3 = high

// ---------------- SETUP ----------------
void setup()
{
    pinMode(LED_PIN, OUTPUT);      // LED as output
    pinMode(BTN1, INPUT_PULLUP);   // Button 1
    pinMode(BTN2, INPUT_PULLUP);   // Button 2

    Serial.begin(9600);            // Serial monitor
}

// ---------------- FUNCTION: READ AVERAGE ----------------
float readTemperature()
{
    long sum = 0;

    for (int i = 0; i < 100; i++)
    {
        sum += analogRead(TEMP_PIN);   // Read ADC
        delay(50);                     // 50ms × 100 = 5 sec
    }

    float avg = sum / 100.0;           // Average value

    return (avg / 1023.0) * 5.0;       // Convert to voltage
}

// ---------------- MAIN LOOP ----------------
void loop()
{
    // -------- BUTTON 1 (ON/OFF) --------
    if (digitalRead(BTN1) == LOW)
    {
        fanOn = !fanOn;    // Toggle state
        delay(300);        // Debounce
    }

    // -------- BUTTON 2 (SENSITIVITY) --------
    if (digitalRead(BTN2) == LOW)
    {
        sensitivity++;
        if (sensitivity > 3) sensitivity = 1;
        delay(300);        // Debounce
    }

    // -------- READ TEMPERATURE --------
    float temp = readTemperature();

    Serial.print("Temp: ");
    Serial.println(temp);

    // -------- FAN LOGIC --------
    if (!fanOn)
    {
        analogWrite(LED_PIN, 0);   // OFF
        Serial.println("Fan OFF");
    }
    else
    {
        int pwm;

        // Adjust based on sensitivity
        if (sensitivity == 1)
            pwm = temp * 30;   // Low sensitivity
        else if (sensitivity == 2)
            pwm = temp * 50;   // Medium
        else
            pwm = temp * 80;   // High

        pwm = constrain(pwm, 0, 255);  // Limit PWM

        analogWrite(LED_PIN, pwm);     // Output PWM

        Serial.print("Fan ON | PWM: ");
        Serial.println(pwm);
    }

    Serial.println("--------------------");
}
```

---

## How It Works

* The Arduino takes 100 samples over 5 seconds
* It averages the value for accuracy
* Button 1 turns the fan ON/OFF
* Button 2 changes sensitivity (low, medium, high)
* PWM signal controls LED brightness (fan speed)

---

## Summary

This system demonstrates:

* ADC sampling and averaging
* PWM control of output
* User interaction with buttons
* Adjustable system behavior using sensitivity levels

The LED brightness accurately represents the speed of a fan based on temperature input.

