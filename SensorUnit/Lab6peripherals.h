/**
 * @file Lab6peripherals.h
 * @brief This file holds the header information for 
 *        peripheral modules
 *
 * @author Gavin Fielder
 * @date 5/12/2018
 *
 * Hardware Connections
 *    Thermistor with 1K ohm resister voltage divider on PE2
 *    UART Tx on PB1
 */

#ifndef SYSTICK_H

//Systick Registers
#define SYSTICK_STCTRL			(*((volatile unsigned long *)0xE000E010))
#define SYSTICK_STRELOAD		(*((volatile unsigned long *)0xE000E014))			
#define SYSTICK_STCURRENT		(*((volatile unsigned long *)0xE000E018))
    
//Onboard LEDs
#define PF1       (*((volatile unsigned long *)0x40025008))
#define PF2       (*((volatile unsigned long *)0x40025010))
#define PF3       (*((volatile unsigned long *)0x40025020))

// Initialize Systick periodic interrupts
// Input: none
// Output: none
// Note: Enables SysTick periodic interrupts at 40 Hz
void SysTick_Init(void);

// Interrupt service routine
// Executed periodically, the actual period
// determined by the current Reload.
void SysTick_Handler(void);

// Initializes Port F for output
// Inputs: None
// Outputs: None
// Notes: Sets all used pins as outputs
void PortF_Init(unsigned int portF_using);


//Turns LED on
void DoorOpenIndicator_On();

//Turns LED off
void DoorOpenIndicator_Off();



#endif
