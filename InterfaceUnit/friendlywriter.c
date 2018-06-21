/**
 * @file friendlyWriter.c
 * @brief This file holds general purpose read/write functions
 *
 * @author Gavin Fielder
 * @date 5/12/2018
 */


#include "stdint.h"
#include "friendlywriter.h"


//Ports
const uint32_t GPIO_BASE[6] = {  0x40004000,    //port A
                                  0x40005000,   //port B
                                  0x40006000,   //port C
                                  0x40007000,   //port D
                                  0x40024000,   //port E
                                  0x40025000 }; //port F

//Writes to an address
void Write(uint32_t address, uint32_t value) {
    (*((volatile unsigned long *)address)) = value;
}

//Writes to an address using friendly code
void FriendlyWrite(uint32_t address, uint32_t value, uint32_t onlyOn) {
    (*((volatile unsigned long *)address)) |= (value & onlyOn);
    (*((volatile unsigned long *)address)) &= (value | (~onlyOn));
}

//Reads an address
unsigned int Read(uint32_t address) {
    return (*((volatile unsigned long *)address));
}
