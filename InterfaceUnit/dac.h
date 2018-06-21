// dac.h
// This software configures DAC output
// Runs on TM4C123
// Program written by: Gavin Fielder
// Date Created: 
// Last Modified: 4/10/2018
// Lab number: 5
// Hardware connections:
//     PA2-PA5 to 5-bit DAC. PA2 is low bit, PA6 high bit

// Header files contain the prototypes for public functions
// this file explains what the module does

// DAC_Init() initializes the specified pins
// DAC_OUT(#) outputs # to the DAC. This will write to the
//            specified pins, enabling a pseudo-analog
//            output in a discrete range on 0-3.3V via the DAC

#ifndef DAC_H
#define DAC_H

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
// Notes: Initializes the specified pins
void DAC_Init(void);

// **************DAC_Out*********************
// output to DAC
// Input: 5-bit data, 0 to 31
// Output: none
// Writes the # to the specified pins, enabling a
// pseudo-analog output in a discrete range in 0-3.3V
void DAC_Out(unsigned long data);

//These functions initialize the specified port with all
//used pins as outputs
void PortA_InitializeOutputs(unsigned int portA_using);

#endif
