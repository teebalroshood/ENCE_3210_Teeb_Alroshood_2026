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
