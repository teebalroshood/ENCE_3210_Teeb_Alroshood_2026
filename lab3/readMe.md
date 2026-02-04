lab3 code


/*
Microprocessor Lab – R-2R DAC Ramp Output and Square Wave Test

This program is used to verify the operation of a 10-bit R-2R Digital-to-Analog Converter (DAC)
and to provide a reference signal for oscilloscope verification.

Functions of the program:
1. Outputs a square wave signal on pin D3 (~1 kHz) for oscilloscope sanity checking.
2. Outputs a continuously increasing 10-bit digital ramp (0 to 1023) on pins D4–D13.
   This digital ramp is converted to an analog staircase waveform by the R-2R ladder.

The staircase waveform confirms:
- Proper digital pin operation
- Correct R-2R ladder wiring
- Successful digital-to-analog conversion

The delay between DAC steps controls the output ramp frequency.
*/

#include <Arduino.h>

/* ---------------------- Pin Configuration ---------------------- */

// Square wave output pin used as a reference signal
const uint8_t SQUARE_PIN = 3;

// 10-bit DAC bus connected to the R-2R ladder
// dacPins[0] corresponds to Least Significant Bit (LSB)
// dacPins[9] corresponds to Most Significant Bit (MSB)
const uint8_t dacPins[10] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

/* ---------------------- DAC Output Function ---------------------- */

/*
writeDAC10()

Outputs a 10-bit digital value to the R-2R ladder.

Parameters:
- value: 10-bit number ranging from 0 to 1023

Each bit of the value is written to its corresponding digital output pin.
*/
void writeDAC10(uint16_t value)
{
    // Ensure that only the lower 10 bits are used
    value &= 0x03FF;

    // Write each bit to its corresponding DAC pin
    for (int bit = 0; bit < 10; bit++)
    {
        digitalWrite(dacPins[bit], (value >> bit) & 1);
    }
}

/* ---------------------- Setup Function ---------------------- */

void setup()
{
    // Configure square wave output pin
    pinMode(SQUARE_PIN, OUTPUT);
    digitalWrite(SQUARE_PIN, LOW);

    // Configure DAC pins as outputs and initialize them to LOW
    for (int i = 0; i < 10; i++)
    {
        pinMode(dacPins[i], OUTPUT);
        digitalWrite(dacPins[i], LOW);
    }
}

/* ---------------------- Main Program Loop ---------------------- */

void loop()
{
    /* -------- Square Wave Generation -------- */

    /*
    A square wave is generated on pin D3 for oscilloscope verification.

    Target frequency: approximately 1 kHz
    Period = 1 ms (1000 microseconds)
    Half period = 500 microseconds
    */

    digitalWrite(SQUARE_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(SQUARE_PIN, LOW);
    delayMicroseconds(500);

    /* -------- DAC Ramp Output -------- */

    /*
    This loop outputs every digital code from 0 to 1023 to the R-2R ladder.

    The resulting analog signal appears as a staircase ramp on the oscilloscope.
    The delay between steps controls the ramp speed and frequency.
    */

    for (uint16_t code = 0; code < 1024; code++)
    {
        // Send digital code to DAC pins
        writeDAC10(code);

        // Delay controls ramp speed
        delayMicroseconds(80);
    }
}
