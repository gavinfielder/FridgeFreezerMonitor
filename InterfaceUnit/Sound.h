// Sound.h
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: Gavin Fielder
// Date Created: 8/25/2013 
// Last Modified: 4/10/2018
// Lab number: 5
// Hardware connections
//     None

#ifndef SOUND_H
#define SOUND_H

#include "stdint.h"

//Systick Registers
#define SYSTICK_STCTRL			(*((volatile unsigned long *)0xE000E010))
#define SYSTICK_STRELOAD		(*((volatile unsigned long *)0xE000E014))			
#define SYSTICK_STCURRENT		(*((volatile unsigned long *)0xE000E018))
    
//Prototypes
void PortE_InitializeOutputs(unsigned int portE_using);

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Called once, with sound initially off
// Input: interrupt period
//           Units to be determined by YOU
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
// Output: none
void Sound_Init(void);


// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Input: interrupt period
//           Units to be determined by YOU
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
//         input of zero disable sound output
// Output: none
void Sound_Play(unsigned long period);


#endif
