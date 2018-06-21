/**
 * @file Lab6.c
 * @brief This file holds the main program for the sensor unit
 *        of the fridge monitoring system
 *
 * @author Gavin Fielder
 * @date 5/12/2018
 *
 * Hardware Connections
 *    Thermistor with 1K ohm resister voltage divider on PE2
 *    UART Tx on PB1
 */

#include "PLL.h"
#include "ADC.h"
#include "tm4c123gh6pm.h"
#include "Lab6peripherals.h"
#include <stdio.h>
#include "math.h"
#include "uart.h"
#include "doorSwitch.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


//Global variables
extern unsigned int ADCMail;
extern unsigned int ADCStatus;
unsigned int Data;      // 12-bit ADC
unsigned int temperature; 
extern int doorStateChanged;

//Function prototypes
unsigned int Temp_Convert(unsigned int data);

//Global 
int alarm; //1 if alarm should be on, 0 if alarm should be off
extern int doorStateChanged; //door switch change event flag
extern unsigned int count; //incremented by systick

//Simple state machine
#define DOOR_CLOSED 0
#define DOOR_OPEN 1
#define DOOR_OPEN_ALARM 2
#define ALARM_TIME_THRESHOLD 25 //number of systick periods
#define TEMPERATURE_THRESHOLD 80 //deg F
int state;

int __main(void){
    
    DisableInterrupts();    //Disable global interrupts
    
    //Initializations
    PLL_Init();          //Bus clock is 80 MHz 
    SysTick_Init();      //5 Hz periodic interrupt
    DoorSwitch_Init(PF,0); //enable door switch with interrupts
    PortF_Init(0xe);     //Enables PF1-3 LEDs
    ADC_Init();          //turn on ADC, set channel to 1 (PE2)
    UART_Init(PB,1,1,Tx); //UART Tx on PB1. 9600 baud rate.
    state = DOOR_CLOSED; //initial state
    count = 0;
    
    EnableInterrupts();     //Enable global interrupts
    
    while(1){     
        //Handle the current state and see if transitions need to be made
        if (state == DOOR_CLOSED) {
            GPIO_PORTF_DATA_R &= ~0x8; //Debug alarm status indicator LED
            alarm = 0; //can be overridden by temperature reading
            DoorOpenIndicator_Off();
            if ((doorStateChanged) && (IsDoorOpen())) {
                count = 0; //start timer
                state = DOOR_OPEN;
            }
        }
        else if (state == DOOR_OPEN) {
            alarm = 0; //can be overridden by temperature reading
            DoorOpenIndicator_On();
            if ((doorStateChanged) && (!(IsDoorOpen())))
                state = DOOR_CLOSED;
            else if (count > ALARM_TIME_THRESHOLD) 
                state = DOOR_OPEN_ALARM;
        }
        else if (state == DOOR_OPEN_ALARM) {
            GPIO_PORTF_DATA_R |= 0x8; //Debug alarm status indicator LED
            alarm = 1;
            if ((doorStateChanged) && (!(IsDoorOpen())))
                state = DOOR_CLOSED;
        }
        //Check status of ADC
        if (ADCStatus) {
            ADCStatus = 0;
            Data = ADCMail;
            temperature = Temp_Convert(Data);
        }
        //If temperature is above the threshold, alarm is always 1
        if (temperature > TEMPERATURE_THRESHOLD)
            alarm = 1;
        //If the FIFO is ready to transmit, send current data
        UART_TransmitIfIdle(alarm, temperature);
    }    
    
    
    return 0;
}




//----------Temp_Convert---------
// Converts 12-bit digital ADC value into useable data
// Input: 12-bit result of ADC conversion
// Output: temperature value in farenheit
unsigned int Temp_Convert(unsigned int data){
    return (3716 * data - 2230522) / 65536;
}
