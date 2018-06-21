// dac.c
// This software configures DAC output
// Runs on TM4C123
// Program written by: Gavin Fielder
// Date Created: 
// Last Modified: 4/10/2018
// Lab number: 5
// Hardware connections:
//     PA2-PA5 to 5-bit DAC. PA2 is low bit, PA6 high bit

#include "tm4c123gh6pm.h"
#include "dac.h"

// **************DAC_Init*********************
// Initialize 5-bit DAC, called once 
// Input: none
// Output: none
// Note: Sets PA2-PA6 as outputs
void DAC_Init(void){
    PortA_InitializeOutputs(0x7c); //0x7c => pins 2-6
}

// **************DAC_Out*********************
// output to DAC
// Input: 5-bit data, 0 to 31 
// Output: none
// Note: outputs on PA2-PA6
void DAC_Out(unsigned long data) {
    GPIO_PORTE_DATA_R = data << 1;
}





