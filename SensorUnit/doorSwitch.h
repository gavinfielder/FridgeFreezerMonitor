/**
 * @file doorSwitch.h
 * @brief This file holds the header information for
 *        the door switch
 *
 * @author Gavin Fielder
 * @date 5/12/2018
 *
 * Hardware Connections
 *    Thermistor with 1K ohm resister voltage divider on PE2
 *    UART Tx on PB1
 */


#ifndef DOORSWITCH_H
#define DOORSWITCH_H

#ifndef GPIO_REGISTER_OFFSETS
    #define GPIO_REGISTER_OFFSETS
    #define DIR_R    0x400
    #define AFSEL_R  0x420
    #define AMSEL_R  0x528
    #define DEN_R    0x51C
    #define PCTL_R   0x52C
    #define LOCK_R   0x520
    #define CR_R     0x524
    #define IM_R     0x410
    #define PUR_R    0x510
    #define IS_R     0x404
    #define IBE_R    0x408
    #define IEV_R    0x40C
    #define ICR_R    0x41C
    #define IM_R     0x410
#endif
                                  

// Initializes PA2 with both-edge triggered interrupts
// Inputs: None
// Outputs: None
// Notes: Sets all used pins as inputs
void DoorSwitch_Init(unsigned int portNum, unsigned int pinNum);

//Interrupt handler
void GPIOPortA_Handler(void);
void GPIOPortB_Handler(void);
void GPIOPortC_Handler(void);
void GPIOPortD_Handler(void);
void GPIOPortE_Handler(void);
void GPIOPortF_Handler(void);

//Returns 1 if door is open, 0 if door is closed
uint8_t IsDoorOpen(void);

#endif

