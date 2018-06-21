/**
 * @file alarm.c
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

#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "alarm.h"

//Holds the sine wave for a 5-bit DAC
const uint16_t NUMBER_OF_DATA_POINTS = 128;
const uint8_t SineWave[NUMBER_OF_DATA_POINTS] = {
    0x10,0x10,0x11,0x12,0x13,0x13,0x14,0x15,
    0x15,0x16,0x17,0x17,0x18,0x19,0x19,0x1a,
    0x1a,0x1b,0x1b,0x1c,0x1c,0x1d,0x1d,0x1e,
    0x1e,0x1e,0x1e,0x1f,0x1f,0x1f,0x1f,0x1f,
    0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1e,0x1e,
    0x1e,0x1e,0x1d,0x1d,0x1c,0x1c,0x1b,0x1b,
    0x1a,0x1a,0x19,0x19,0x18,0x17,0x17,0x16,
    0x15,0x15,0x14,0x13,0x13,0x12,0x11,0x10,
    0x10,0xf,0xe,0xd,0xc,0xc,0xb,0xa,
    0xa,0x9,0x8,0x8,0x7,0x6,0x6,0x5,
    0x5,0x4,0x4,0x3,0x3,0x2,0x2,0x1,
    0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,
    0x1,0x1,0x2,0x2,0x3,0x3,0x4,0x4,
    0x5,0x5,0x6,0x6,0x7,0x8,0x8,0x9,
    0xa,0xa,0xb,0xc,0xc,0xd,0xe,0xf
};
//Sets frequency of sound
const uint32_t period = 1420;

//Tracks the next data point
int next;
//Masks the output to toggle signal on/off
uint32_t outmask;

//Initializes DAC and SysTick
void InitializeAlarm(){ 
    
    int delay;
    unsigned int portE_using = 0x3e;
    
    //Initialize PE1-5 as 5-bit DAC output
    SYSCTL_RCGC2_R |= 0x10; //send clock signal 
    delay = 0;
    GPIO_PORTE_DIR_R |= portE_using; //set bits (output)
    GPIO_PORTE_AFSEL_R &= ~portE_using; //clear bits 
    GPIO_PORTE_AMSEL_R &= ~portE_using; //clear bits 
    GPIO_PORTE_DEN_R |= portE_using; //set bits 
    
    //Initialize SysTick with periodic interrupts
    NVIC_ST_CTRL_R = 0; // disable SysTick during setup
    NVIC_ST_RELOAD_R = period - 1;
    NVIC_ST_CURRENT_R = 0; // any write to current clears it
    NVIC_PRI3_R = (NVIC_PRI3_R&0x00FFFFFF)|0x40000000; //priority 2
    NVIC_EN0_R |= 0x00008000; //enable interrupt 15 (SysTick)
    NVIC_ST_CTRL_R = 0x07; // enable with system clock and interrupts
    
    //initial value
    next = 0;
}

//Turns off alarm
void AlarmOff(void) {
    NVIC_ST_RELOAD_R = 0xFFFFFF;
    outmask = 0;
}

//Turns on alarm
void AlarmOn(void) {
    NVIC_ST_RELOAD_R = period - 1;
    outmask = 0x3e;
}


// Interrupt service routine
// Executed periodically, the actual period
// determined by the current Reload.
void SysTick_Handler(void){
    DAC_Out(SineWave[next] & (outmask >> 1)); //output next value
    next = next + 1; //set next index value
    if (next >= NUMBER_OF_DATA_POINTS) next = 0; //wrap around
}

// **************DAC_Out*********************
// output to DAC
// Input: 5-bit data, 0 to 31 
// Output: none
// Note: outputs on PA2-PA6
void DAC_Out(unsigned long data) {
    GPIO_PORTE_DATA_R = data << 1;
}


