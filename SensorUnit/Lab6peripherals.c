/**
 * @file Lab6peripherals.c
 * @brief This file holds peripheral functions for 
 *        the fridge monitoring system.
 *
 * @author Gavin Fielder
 * @date 5/12/2018
 *
 * Hardware Connections
 *    Thermistor with 1K ohm resister voltage divider on PE2
 *    UART Tx on PB1
 */


#include "Lab6peripherals.h"
#include "tm4c123gh6pm.h"
#include "ADC.h"
#include "friendlywriter.h"

//Global variables
unsigned long ADCMail;
unsigned int ADCStatus;
unsigned int count;

// **************SysTick_Init*********************
// Initialize Systick periodic interrupts
// Input: none
// Output: none
// Note: Enables SysTick periodic interrupts at 40 Hz
void SysTick_Init(){
    SYSTICK_STCTRL = 0; // disable SysTick during setup
    SYSTICK_STRELOAD = 0xF42400 - 1; //5 Hz at clock speed 80 MHz
    SYSTICK_STCURRENT = 0; // any write to current clears it
    NVIC_PRI3_R = (NVIC_PRI3_R&0x00FFFFFF)|0x40000000; //priority 2
    NVIC_EN0_R |= 0x00008000; //enable interrupt 15 (SysTick)
    SYSTICK_STCTRL = 0x07; // enable with system clock and interrupts
}

// Interrupt service routine
// Executed periodically, the actual period
// determined by the current Reload.
void SysTick_Handler(void){
    GPIO_PORTF_DATA_R |= 0x04; //Turn on LED
    ADCMail = ADC_In(); //get ADC input
    ADCStatus = 1; //flag that new output is ready
    GPIO_PORTF_DATA_R &= ~0x04; //Turn off LED
    count = count + 1;
}

// Initializes Port F for output
// Inputs: None
// Outputs: None
// Notes: Sets all used pins as outputs
void PortF_Init(unsigned int portF_using){
    SYSCTL_RCGC2_R |= 0x20; //send clock signal
    portF_using = portF_using; //delay
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R |= portF_using; //set bits 
	GPIO_PORTF_PCTL_R &= ~portF_using; //clear bits 
    GPIO_PORTF_DIR_R |= portF_using; //set bits for output
	GPIO_PORTF_PUR_R |= portF_using; //set bits 
    GPIO_PORTF_AFSEL_R &= ~portF_using; //clear bits 
    GPIO_PORTF_AMSEL_R &= ~portF_using; //clear bits 
    GPIO_PORTF_DEN_R |= portF_using; //set bits 
}


//Turns LED on
void DoorOpenIndicator_On() {
    GPIO_PORTF_DATA_R |= 0x02;
}

//Turns LED off
void DoorOpenIndicator_Off() {
    GPIO_PORTF_DATA_R &= ~0x02;
}
