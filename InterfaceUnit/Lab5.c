/**
 * @file Lab5.c This file holds the main program for the 
 *              interface unit of the fridge monitoring system.
 *
 * @author Gavin Fielder
 * @date 5/12/2018
 *
 * Hardware Connections
 *    UART Rx on PB0
 *    5-bit DAC connected to PE1-5, output to audio device
 *          1.5 kohm PE5
 *          3.0 kohm PE4
 *          6.0 kohm PE3
 *         12.0 kohm PE2
 *         24.0 kohm PE1
 *    Nokia LCD on Port A
 *          // Red SparkFun Nokia 5110 (LCD-10168)
 *          // -----------------------------------
 *          // Signal        (Nokia 5110) LaunchPad pin
 *          // 3.3V          (VCC, pin 1) power
 *          // Ground        (GND, pin 2) ground
 *          // SSI0Fss       (SCE, pin 3) connected to PA3
 *          // Reset         (RST, pin 4) connected to PA7
 *          // Data/Command  (D/C, pin 5) connected to PA6
 *          // SSI0Tx        (DN,  pin 6) connected to PA5
 *          // SSI0Clk       (SCLK, pin 7) connected to PA2
 *          // back light    (LED, pin 8) not connected
 */



#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "stdint.h"
#include "Nokia5110.h"
#include "alarm.h"
#include "uart.h"
#include "stdio.h"


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // Low power wait

int __main(void){     
    char str[16];
    uint8_t data_in = 0;
    
    DisableInterrupts();
    
    UART_Init(PB,0,1,Rx); //Select PB0, UART1, Rx
    Nokia5110_Init();     //LCD
    PLL_Init();           // bus clock at 80 MHz
    InitializeAlarm();    //Initialize DAC, SysTick
    Nokia5110_SetCursor(0,0);
    Nokia5110_Clear();
    
    EnableInterrupts(); //Enable global interrupts

    while(1){                
        //Check if new data received
        if (UART_NewDataAvailable()) {
            Nokia5110_Clear();
            Nokia5110_SetCursor(0,0);
            data_in = UART_Read();
            //Get alarm status (bit 7)
            if (data_in & 0x80)
                AlarmOn();
            else
                AlarmOff();
            //Read temperature data (bits 0-6)
            data_in &= ~0x80;
            //print data
            sprintf(str, "%u degF", data_in);
            Nokia5110_OutString(str);
        }
    }             
}


