/**
 * @file ADC.c
 * @brief This file holds the ADC initialization
 *
 * @author Gavin Fielder
 * @date 5/12/2018
 *
 * Hardware Connections
 *    Thermistor with 1K ohm resister voltage divider on PE2
 *    UART Tx on PB1
 */

#include "tm4c123gh6pm.h"


// This initialization function 
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain1 (PE2)
// SS3 interrupts: flag set on completion but no interrupt requested
void ADC_Init(void){ 
    unsigned int PE2;
    int delay;
    SYSCTL_RCGC2_R |= 0x10; 
    PE2 = 0x04;
    //Configure as analog input
    GPIO_PORTE_DIR_R &= ~PE2;
    GPIO_PORTE_AFSEL_R |= PE2;
    GPIO_PORTE_DEN_R &= ~PE2;     
    GPIO_PORTE_PCTL_R &= ~PE2; 
    GPIO_PORTE_AMSEL_R |= PE2;

    SYSCTL_RCGCADC_R |= 1;
    SYSCTL_RCGCADC_R &= ~2;
    for (delay = 0; delay < 50; delay++);  //needs longer delay to not hard fault
    ADC0_PC_R = 0x1; 
    ADC0_SSPRI_R = 0x3210;      //8) Set sequencer priority
    ADC0_ACTSS_R &= ~0x8;       //9) Disable sequencer 3
    ADC0_EMUX_R &= ~0xF000;     //10) Set software trigger
    ADC0_SSMUX3_R = 0x1;        //11) Select channel AIN1
    ADC0_SSCTL3_R = 0x06;       //12) enable interrupt, flag last step
    ADC0_ACTSS_R |= 0x08;       //13) Enable sequencer 3

}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned int ADC_In(void){  
    unsigned int data;
    ADC0_PSSI_R |= 0x08; //Start ADC
    while ((ADC0_RIS_R & 0x08)==0);
    data = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R |= 0x08; //Clear flag
    return data;
}


