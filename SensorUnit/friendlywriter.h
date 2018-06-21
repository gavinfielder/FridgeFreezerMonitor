/**
 * @file friendlyWriter.h
 * @brief This file holds general purpose read/writer functions
 *
 * @author Gavin Fielder
 * @date 5/12/2018
 */

#ifndef FRIENDLYWRITER_H
#define FRIENDLYWRITER_H

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
    #define DATA_R   0x3FC
#endif
    

//Writes to an address
void Write(uint32_t address, uint32_t value) ;

//Writes to an address using friendly code
void FriendlyWrite(uint32_t address, uint32_t value, uint32_t onlyOn);

//Reads an address
unsigned int Read(uint32_t address);

#endif
