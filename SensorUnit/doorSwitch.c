/**
 * @file doorSwitch.c
 * @brief This file holds the generalized door switch functions
 *
 * @author Gavin Fielder
 * @date 5/12/2018
 *
 * Hardware Connections
 *    Thermistor with 1K ohm resister voltage divider on PE2
 *    UART Tx on PB1
 */


#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "friendlywriter.h"

//Globals
int doorStateChanged;
unsigned int selected_port;
unsigned int selected_pin;

//Ports
extern const uint32_t GPIO_BASE[6];


// Initializes a pin with both-edge triggered interrupts
void DoorSwitch_Init(unsigned int portNum, unsigned int pinNum){    
    int delay;
    unsigned int GPIO_Using = 1 << pinNum;
    SYSCTL_RCGCGPIO_R |= ( 1 << portNum);
    delay = 0;
    //Configure as GPIO
    Write(GPIO_BASE[portNum] + LOCK_R, GPIO_LOCK_KEY);
    FriendlyWrite(GPIO_BASE[portNum] + CR_R, GPIO_Using, GPIO_Using);
    FriendlyWrite(GPIO_BASE[portNum] + DIR_R, 0, GPIO_Using);
    FriendlyWrite(GPIO_BASE[portNum] + AFSEL_R, 0, GPIO_Using);
    FriendlyWrite(GPIO_BASE[portNum] + DEN_R, GPIO_Using, GPIO_Using);
    FriendlyWrite(GPIO_BASE[portNum] + PCTL_R, 0, (0xF << (pinNum*4)));
    FriendlyWrite(GPIO_BASE[portNum] + AMSEL_R, 0, GPIO_Using);
    
    //Configure Interrupts
    FriendlyWrite(GPIO_BASE[portNum] + IM_R, 0, GPIO_Using); // Disarm the interrupt
    FriendlyWrite(GPIO_BASE[portNum] + PUR_R, GPIO_Using, GPIO_Using); //enable pull up resistor
    FriendlyWrite(GPIO_BASE[portNum] + IS_R, 0, GPIO_Using); // is edge-sensitive
    FriendlyWrite(GPIO_BASE[portNum] + IBE_R, GPIO_Using, GPIO_Using); // is both edges
    FriendlyWrite(GPIO_BASE[portNum] + IEV_R, 0, GPIO_Using); // 
    Write(GPIO_BASE[portNum] + ICR_R, GPIO_Using); // clear flag
    FriendlyWrite(GPIO_BASE[portNum] + IM_R, GPIO_Using, GPIO_Using); //Enable the interrupt
    
    
    //Enable NVIC
    if (portNum == PF) 
        NVIC_EN0_R |= (1 << 30);
    else 
        NVIC_EN0_R |= (1 << portNum);
    
    //Set priority 5
    if (portNum <= 3)
        FriendlyWrite(NVIC_PRI0_R, (5 << 5) << (portNum * 8), (0x7 << 5) << (portNum * 8));
    else if (portNum == 4)
        FriendlyWrite(NVIC_PRI1_R, (5 << 5), (0xe << 5));
    else 
        FriendlyWrite(NVIC_PRI7_R, (5 << 5) << (24), (0x7 << 5) << (24));
    
    //Set initial flag value
    doorStateChanged = 0;
    
    //Save settings
    selected_port = portNum;
    selected_pin = pinNum;
}


//Returns 1 if door is open, 0 if door is closed
//Also resets doorStateChanged flag.
uint8_t IsDoorOpen() {
    if (Read(GPIO_BASE[selected_port] + DATA_R) & ( 1 << selected_pin) ) {
        //reading 1, door is closed
        doorStateChanged = 0;
        return 0;
    }
    else {
        doorStateChanged = 0;
        return 1;
    }
}




//Comment out the handlers that are unneeded

//Interrupt handler
//void GPIOPortA_Handler(void) {
//    doorStateChanged = 1;
//    GPIO_PORTA_ICR_R |= (1 << selected_pin);
//}

////Interrupt handler
//void GPIOPortB_Handler(void) {
//    doorStateChanged = 1;
//    GPIO_PORTB_ICR_R |= (1 << selected_pin);
//}

////Interrupt handler
//void GPIOPortC_Handler(void) {
//    doorStateChanged = 1;
//    GPIO_PORTC_ICR_R |= (1 << selected_pin);
//}

////Interrupt handler
//void GPIOPortD_Handler(void) {
//    doorStateChanged = 1;
//    GPIO_PORTD_ICR_R |= (1 << selected_pin);
//}

////Interrupt handler
//void GPIOPortE_Handler(void) {
//    doorStateChanged = 1;
//    GPIO_PORTE_ICR_R |= (1 << selected_pin);
//}

//Interrupt handler
void GPIOPortF_Handler(void) {
    doorStateChanged = 1;
    GPIO_PORTF_ICR_R |= (1 << selected_pin);
}


