// Piano.h
// This software configures the off-board piano keys
// Runs on TM4C123
// Program written by: Gavin Fielder
// Date Created: 
// Last Modified: 4/10/2018
// Lab number: 5
// Hardware connections
//     PD0-PD3 switch inputs

// Header files contain the prototypes for public functions
// this file explains what the module does

// **************Piano_Init*********************
// Initialize piano key inputs, called once 
// Input: none 
// Output: none
void Piano_Init(void);

// **************Piano_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 0 to 16 depending on keys
// 0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2, 0x08 is just Key3
unsigned long Piano_In(void);

//These functions initialize the specified port with all
//used pins as both-edge triggered interrupts
void PortD_InitializeInputs(unsigned int portD_using);
