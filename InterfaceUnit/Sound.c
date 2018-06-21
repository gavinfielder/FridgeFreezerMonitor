// Sound.c, 
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: Gavin Fielder
// Date Created: 8/25/2013 
// Last Modified: 4/10/2018
// Lab number: 5
// Hardware connections
//     None

#include "tm4c123gh6pm.h"
#include "dac.h"
#include "Sound.h"
#include "stdint.h"

//Holds the sine wave for a 5-bit DAC
const uint16_t NUMBER_OF_DATA_POINTS = 128;
const uint8_t SineWave[NUMBER_OF_DATA_POINTS] = {
    0x10,0x10,0x11,0x12,0x13,0x13,0x14,0x15,
    0x15,0x16,0x17,0x17,0x18,0x19,0x19,0x1a,
    0x1a,0x1b,0x1b,0x1c,0x1c,0x1d,0x1d,0x1e,
    0x1e,0x1e,0x1e,0x1f,0x1f,0x1f,0x1f,0x1f,
    0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1e,0x1e,
    0x1e,0x1e,0x1d,0x1d,0x1c,0x1c,0x1b,0x1b,
    0x1a,0x1a,0x19,0x19,0x18,0x17,0x17,0x16,
    0x15,0x15,0x14,0x13,0x13,0x12,0x11,0x10,
    0x10,0xf,0xe,0xd,0xc,0xc,0xb,0xa,
    0xa,0x9,0x8,0x8,0x7,0x6,0x6,0x5,
    0x5,0x4,0x4,0x3,0x3,0x2,0x2,0x1,
    0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,
    0x1,0x1,0x2,0x2,0x3,0x3,0x4,0x4,
    0x5,0x5,0x6,0x6,0x7,0x8,0x8,0x9,
    0xa,0xa,0xb,0xc,0xc,0xd,0xe,0xf
};

//Tracks the next data point
int next;
//Masks the output to toggle signal on/off
uint32_t outmask;

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Input: Initial interrupt period
//           Units to be determined by YOU
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
// Output: none
void Sound_Init(){
    SYSTICK_STCTRL = 0; // disable SysTick during setup
    SYSTICK_STRELOAD = 0xFFFFFF;
    SYSTICK_STCURRENT = 0; // any write to current clears it
    NVIC_PRI3_R = (NVIC_PRI3_R&0x00FFFFFF)|0x40000000; //priority 2
    NVIC_EN0_R |= 0x00008000; //enable interrupt 15 (SysTick)
    SYSTICK_STCTRL = 0x07; // enable with system clock and interrupts
}

// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Input: interrupt period
//           Units to be determined by YOU
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
//         input of zero disables sound output
// Output: none
void Sound_Play(unsigned long period){
    if (period > 0) {
        outmask = 0x7c; //enable signal output on pins 2-6
        NVIC_ST_RELOAD_R = period - 1;
    }
    else //passing zero indicates no signal should be output
        outmask = 0x00; //forces no signal
}

// Interrupt service routine
// Executed periodically, the actual period
// determined by the current Reload.
void SysTick_Handler(void){
    GPIO_PORTE_DATA_R ^= 0x02; //debug heartbeat toggle
    DAC_Out(SineWave[next] & (outmask >> 2)); //output next value
    next = next + 1; //set next index value
    if (next >= NUMBER_OF_DATA_POINTS) next = 0; //wrap around
}

// Initializes Port E for output
// Inputs: None
// Outputs: None
// Notes: Sets all used pins as outputs
void PortE_InitializeOutputs(unsigned int portE_using){ 
    SYSCTL_RCGC2_R |= 0x10; //send clock signal
    portE_using = portE_using; //delay
    GPIO_PORTE_DIR_R |= portE_using; //set bits (output)
    GPIO_PORTE_AFSEL_R &= ~portE_using; //clear bits 
    GPIO_PORTE_AMSEL_R &= ~portE_using; //clear bits 
    GPIO_PORTE_DEN_R |= portE_using; //set bits 
}
