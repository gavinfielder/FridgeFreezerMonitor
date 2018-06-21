// Piano.c
// This software configures the off-board piano keys
// Runs on TM4C123
// Program written by: Gavin Fielder
// Date Created: 
// Last Modified: 4/10/2018
// Lab number: 5
// Hardware connections
//     PD0-PD3 switch inputs

#include "tm4c123gh6pm.h"
#include "Piano.h"
#include "stdint.h"
#include "Sound.h"

//Defines which pins are used on the port
#define USING_PINS 0x0f //pins 0-3

//Holds piano switches input
uint32_t input;

// **************Piano_Init*********************
// Initialize piano key inputs, called once 
// Input: none 
// Output: none
// Note: Initializes PD0-PD3 as inputs
void Piano_Init(void){ 
    PortD_InitializeInputs(USING_PINS);
}

// **************Piano_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 
// 0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2 and 0x08 is just key3
// Note: For multiple key inputs, returns 0
unsigned long Piano_In(void){ 
    return ((GPIO_PORTD_DATA_R & USING_PINS) >> 0);
}

//Handles Port D interrupts
void GPIOPortD_Handler(void) {
    input = Piano_In();
    switch (input) {
       case 0x01:
           Sound_Play(1420); //A
           break;
       case 0x02:
           Sound_Play(1265); //B
           break;
       case 0x04:
           Sound_Play(2385); //C
           break;
       case 0x08:
           Sound_Play(2126); //D
           break;
       default:
           Sound_Play(0); //No sound
           break;
   }
}

// Initializes Port D with both-edge triggered interrupts
// Inputs: None
// Outputs: None
// Notes: Sets all used pins as inputs
void PortD_InitializeInputs(unsigned int portD_using){    
    SYSCTL_RCGC2_R |= 0x8; 
    portD_using = portD_using; //delay
    //Configure as GPIO
    GPIO_PORTD_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTD_CR_R |= portD_using;
    GPIO_PORTD_DIR_R &= ~portD_using;
    GPIO_PORTD_AFSEL_R &= ~portD_using;
    GPIO_PORTD_DEN_R |= portD_using;     
    GPIO_PORTD_PCTL_R &= ~portD_using; 
    GPIO_PORTD_AMSEL_R &= ~portD_using;
    
    //Configure Interrupts
    GPIO_PORTD_IM_R &=~portD_using;		 // Disarm the interrupt
    GPIO_PORTD_PUR_R |= portD_using;     // enable weak pull-up
    GPIO_PORTD_IS_R &= ~portD_using;     // is edge-sensitive
    GPIO_PORTD_IBE_R |= portD_using;     // is both edges
    GPIO_PORTD_IEV_R &= ~portD_using;    // falling edge event
    GPIO_PORTD_ICR_R = portD_using;      // clear flag
    GPIO_PORTD_IM_R |= portD_using;      // arm interrupt
    NVIC_PRI0_R = (NVIC_PRI0_R&0x1FFFFFFF)|0xA0000000; // (g) priority 5
    NVIC_EN0_R |= (1<<3); //enable interrupt 3 (Port D)
}


