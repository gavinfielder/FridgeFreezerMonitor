/**
 * @file rgf_gpio.c
 *
 * @brief Holds Implementation of Gavin's GPIO Interface
 * 
 * @author Gavin Fielder
 * @date 12/3/2017
 */

#include <stdint.h>
#include <string.h>
#include "rgf_gpio.h"

//Ports
const uint32_t PORTS_BASE[6] = {  0x40004000,   //port A
                                  0x40005000,   //port B
                                  0x40006000,   //port C
                                  0x40007000,   //port D
                                  0x40024000,   //port E
                                  0x40025000 }; //port F

//Default configuration settings
const gpio_portConfig_t GPIO_DEFAULT_CONFIG = {0xFF,0xFF,0x0};

/**
 * Configures a port with the default settings
 *
 * @param  portChar  A char specifying the port, e.g. 'A'
 */
void gpio_initializePort(char portChar) {
  uint8_t port = portChar - 'A';
  if (port > 5) return; //abort
  gpio_configurePort_defaultValues(port);
}

/**
 * Configures a port with the default settings
 *
 * @param  port  the port to configure
 */
void gpio_configurePort_defaultValues(uint8_t port) {
  gpio_configurePort(port, GPIO_DEFAULT_CONFIG);
}

/**
 * Configures a port
 *
 * @param  port    the port to configure
 * @param  config  the GPIO configuration settings (DIR, PUR, PDR) 
 */
void gpio_configurePort(uint8_t port, gpio_portConfig_t config) {
  //Step 1: RCGC  - send clock signal
  uint32_t p = 1 << port;   //see if this works without casting port to uint_xx
  SYSCTL_RCGCGPIO |= p;    //set the proper bit
  
  //Step 2: LOCK - unlock port 
  writeToMemory(PORTS_BASE[port]+PORTOFFSETS_LOCK, GPIO_PORT_LOCK_KEY);

  //Step 3: CR - enable commit
  writeToMemory(PORTS_BASE[port]+PORTOFFSETS_CR, 0xFF);
  
  //Step 4: AMSEL - disable analog
  writeToMemory(PORTS_BASE[port]+PORTOFFSETS_AMSEL, 0x00);
  
  //Step 5: PCTL - clear PCTL
  writeToMemory(PORTS_BASE[port]+PORTOFFSETS_PCTL, 0x00);
  
  //Step 6: DIR - set direction
  writeToMemory(PORTS_BASE[port]+PORTOFFSETS_DIR, config.dir);
  
  //Step 7: AFSEL - disable alternate functions
  writeToMemory(PORTS_BASE[port]+PORTOFFSETS_AFSEL, 0x00);
  
  //Step 8: PDR and PUR - set pull-down and pull-up resistors
  writeToMemory(PORTS_BASE[port]+PORTOFFSETS_PDR, config.pdr);
  writeToMemory(PORTS_BASE[port]+PORTOFFSETS_PUR, config.pur);
  
  //Step 9: DEN  - digital enable
  writeToMemory(PORTS_BASE[port]+PORTOFFSETS_DEN, 0xFF);
}

/**
 * Writes the specified data to the specified memory location.
 * Helper function for initialization
 * 
 * @param  address  the address to write to
 * @param  data     the data to write
 */
void writeToMemory(uint32_t address, uint32_t data) {
  *((volatile unsigned int *)address) = data;
}

/**
 * Sets the specified pin to either input or output. 
 *
 * @param  pin      a 3-character string specifying the pin, e.g. "PA7"
 * @param  IOFlag    input or output
 */
void gpio_setIO(char pin[3], uint8_t IOFlag) {
  uint8_t port = pin[1] - 'A';
  uint8_t inputOk = 1;
  if (port > 5) {
    inputOk = 0;
  }
  if (inputOk) {
    uint32_t pinNumber = pin[2] - '0';
    if (pinNumber > 7) {
      inputOk = 0;
    }
    if (inputOk) {
      uint32_t fm = 1 << pinNumber;
      uint32_t address = PORTS_BASE[port] + PORTOFFSETS_DIR;
      if (IOFlag == DIR_INPUT) { //input. Clear specified pin
        fm = ~fm;
        *((volatile unsigned int *)address) &= fm;
      }
      else { //output. Set specified pin
        *((volatile unsigned int *)address) |= fm;
      }
    }
  }
}

/**
 * Reads the specified input pin. Returns 1 or 0.
 * 
 * @param  pin    a 3-character string specifying the pin, e.g. "PA7"
 * 
 * @return  the bit of the input, 1 or 0
 */
uint8_t gpio_read(char pin[3]) {
  uint8_t port = pin[1] - 'A';
  uint8_t pinNumber = pin[2] - '0';
  uint32_t addr = 0;
  uint8_t data = 0;
  if (port > 5) return 0; //abort
  if (pinNumber > 7) return 0; //abort
  addr = PORTS_BASE[port] + PORTOFFSETS_DATA;
  data = (*((volatile unsigned int *)addr)) & (1 << pinNumber);
  data = data >> pinNumber; //put filtered bit into lsb
  return data;
}

/**
 * Writes to the specified output pin. 
 * 
 * @param  pin    a 3-character string specifying the pin, e.g. "PA7"
 * @param  val    1 or 0
 */
void gpio_write(char pin[3], uint8_t val) {
  uint8_t port = pin[1] - 'A';
  uint8_t pinNumber = pin[2] - '0';
  uint32_t addr = 0;
  uint8_t data = 0;
  uint8_t filter = 0;
  if (port > 5) return; //abort
  if (pinNumber > 7) return; //abort
  addr = PORTS_BASE[port] + PORTOFFSETS_DATA;
  data = (*((volatile unsigned int *)addr)); //load
  filter = 1 << pinNumber;
  if (val == 0)
    data = data & (~filter); //clear specified bit
  else 
    data = data | filter; //set specified bit
  (*((volatile unsigned int *)addr)) = data; //store
}




