/**
 * @file alarm.h
 * @brief This file holds functions for the alarm system
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



#ifndef ALARM_H
#define ALARM_H

#include "stdint.h"
#include "tm4c123gh6pm.h"

//Initializes DAC and SysTick
void InitializeAlarm(void);

//Outputs data to DAC
void DAC_Out(unsigned long data);

//Turns off alarm
void AlarmOff(void);

//Turns on alarm
void AlarmOn(void);



#endif
