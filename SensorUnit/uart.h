/**
 * @file uart.h
 * @brief This file holds the header for generalized UART
 *        initialization
 *
 * @author Gavin Fielder
 * @date 5/12/2018
 *
 * Hardware Connections
 *    Thermistor with 1K ohm resister voltage divider on PE2
 *    UART Tx on PB1
 */


#ifndef UART_H
#define UART_H

#include "stdint.h"

//Port numbers
#ifndef PORTNUM_DEFINES
    #define PORTNUM_DEFINES
    #define PA 0
    #define PB 1
    #define PC 2
    #define PD 3
    #define PE 4
    #define PF 5
#endif

//Directions
#define Tx 1
#define Rx 0

/**
 * Generalized UART Initialization
 *
 * @param  portNum   PA, PB, PC, PD, PE, or PF (defined in header)
 * @param  pinNum    0-7 pin select
 * @param  UARTNum   0-7 UART module select
 * @param  dir       Tx or Rx (defined in header)
 */
void UART_Init(unsigned int portNum, unsigned int pinNum, 
               unsigned int UARTNum, unsigned int dir);


//Transmits data if the UART is idle
void UART_TransmitIfIdle(uint8_t alarmStatus, uint8_t temperature);

//Writes to an address
void Write(uint32_t address, uint32_t value);

//Writes to an address using friendly code
void FriendlyWrite(uint32_t address, uint32_t value, uint32_t onlyOn);

//Reads an address
unsigned int Read(uint32_t address);



#endif

