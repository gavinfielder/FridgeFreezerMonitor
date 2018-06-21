/**
 * @file uart.c
 * @brief This file holds generalized initialization for
 *        UART simplex communication
 *
 * @author Gavin Fielder
 * @date 5/12/2018
 */



#include "stdint.h"
#include "tm4c123gh6pm.h"
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "friendlywriter.h"


//Ports
extern const uint32_t GPIO_BASE[6];
 
//UART module base addresses
const uint32_t UART_BASE[8] = { 0x4000C000,   //UART0
                                0x4000D000,   //UART1
                                0x4000E000,   //UART2
                                0x4000F000,   //UART3
                                0x40010000,   //UART4
                                0x40011000,   //UART5
                                0x40012000,   //UART6
                                0x40013000 }; //UART7
//UART Register offsets
#define CTL_R    0x030
#define DR_R     0x000
#define IBRD_R   0x024
#define FBRD_R   0x028
#define LCRH_R   0x02C
#define CC_R     0xFC8
#define FR_R     0x018
                                
//Saved UART Number
unsigned int selected_UART;

/**
 * Generalized UART Initialization
 *
 * @param  portNum   PA, PB, PC, PD, PE, or PF (defined in header)
 * @param  pinNum    0-7 pin select
 * @param  UARTNum   0-7 UART module select
 * @param  dir       Tx or Rx (defined in header)
 */
void UART_Init(unsigned int portNum, unsigned int pinNum, 
               unsigned int UARTNum, unsigned int dir) {

    int delay;
    unsigned int GPIO_Using = 1 << pinNum;
    unsigned int PCTLValue = 1;
    unsigned int RCGCAddress;
    unsigned int DIRValue = dir << pinNum;
    if (UARTNum < 3) 
        RCGCAddress = 0x400FE104; //use legacy SYSCTL_RCGC1_R
    else
        RCGCAddress = 0x400FE618; //current SYSCTL_RCGCUART_R
    
    //PC4 and PC5 on UART1 get PCTL of 2
    if (portNum == 2 && UARTNum == 1 && (pinNum == 4 || pinNum == 5))
        PCTLValue = 2;
    
    //Send clock signals
    FriendlyWrite(RCGCAddress, 1 << UARTNum, 1 << UARTNum); //Enable UART clock
    for (delay = 0; delay < 100; delay++);
    SYSCTL_RCGCGPIO_R |= (1 << portNum); //Enable GPIO Clock
    for (delay = 0; delay < 10; delay++);
    
    //Initialize GPIO pin
    FriendlyWrite(GPIO_BASE[portNum] + DIR_R, DIRValue, GPIO_Using); //DIR
    FriendlyWrite(GPIO_BASE[portNum] + AFSEL_R, 1 << pinNum, GPIO_Using); //AFSEL
    FriendlyWrite(GPIO_BASE[portNum] + AMSEL_R, 0, GPIO_Using); //AMSEL
    FriendlyWrite(GPIO_BASE[portNum] + DEN_R, 1 << pinNum, GPIO_Using); //DEN
    FriendlyWrite(GPIO_BASE[portNum] + PCTL_R, PCTLValue << (pinNum * 4), (0xF << (pinNum * 4))); //PCTL
    
    //Initialize UART at 9600 baud rate for 80 MHz clock
    FriendlyWrite(UART_BASE[UARTNum] + CTL_R, 0, 1); //disable during setup
    Write(UART_BASE[UARTNum] + IBRD_R, 520); //integer part of baud rate clock divisor
    Write(UART_BASE[UARTNum] + FBRD_R, 53); //fraction part of baud rate clock divisor
    Write(UART_BASE[UARTNum] + LCRH_R, 0x70); //enable FIFO and set word length to 8
    Write(UART_BASE[UARTNum] + CC_R, 0); //select system clock source
    FriendlyWrite(UART_BASE[UARTNum] + CTL_R, 1, 1); //enable UART
    
    //Save settings
    selected_UART = UARTNum;
}    
                                  
//Transmits data if the UART is idle
void UART_TransmitIfIdle(uint8_t alarmStatus, uint8_t temperature) {
    uint8_t data = (alarmStatus << 7) | temperature;
    if (Read(UART_BASE[selected_UART] + FR_R) & 0x80) { //if FIFO empty
        (*((volatile unsigned char *)(UART_BASE[selected_UART] + DR_R))) = data;
    }
}


//Returns 1 if data exists in FIFO, 0 otherwise
uint8_t UART_NewDataAvailable() {
    if ((Read(UART_BASE[selected_UART] + FR_R) & 0x10) == 0) {
        //FIFO NOT empty. Return 1 for new data detected
        return 1;
    }
    else return 0;
}

//Extracts data from FIFO
uint8_t UART_Read(void) {
    return (Read(UART_BASE[selected_UART] + DR_R) & 0xFF);

}










