/**
 * @file rgf_gpio.h
 *
 * @brief Holds Declarations for Gavin's GPIO Interface
 * 
 * @author Gavin Fielder
 * @date 12/3/2017
 */
#ifndef RGF_GPIO_H
#define RGF_GPIO_H


//System control
#define SYSCTL_RCGCGPIO (*((volatile unsigned int *)0x400FE608))
  
//Offsets
#define PORTOFFSETS_DATA     0x3FC
#define PORTOFFSETS_DIR      0x400
#define PORTOFFSETS_AFSEL    0x420
#define PORTOFFSETS_PUR      0x510
#define PORTOFFSETS_PDR      0x514
#define PORTOFFSETS_DEN      0x51C
#define PORTOFFSETS_LOCK    0x520
#define PORTOFFSETS_CR      0x524
#define PORTOFFSETS_AMSEL    0x528
#define PORTOFFSETS_PCTL    0x52C

//Other
#define DIR_INPUT 0
#define DIR_OUTPUT 1
#define GPIO_PORT_LOCK_KEY  0x4C4F434B

//Defines a structure for passing GPIO configuration parameters
typedef struct gpio_portConfig_struct {
  uint32_t dir;
  uint32_t pur;
  uint32_t pdr;
} gpio_portConfig_t;



/**
 * Configures a port
 *
 * @param  port    the port to configure
 * @param  config  the GPIO configuration settings (DIR, PUR, PDR) 
 */
void gpio_configurePort(uint8_t port, gpio_portConfig_t config);

/**
 * Configures a port with the default settings
 *
 * @param  port  the port to configure
 */
void gpio_configurePort_defaultValues(uint8_t port);

/**
 * Configures a port with the default settings
 *
 * @param  portChar  A char specifying the port, e.g. 'A'
 */
void gpio_initializePort(char portChar);

/**
 * Sets the specified pin to either input or output. 
 *
 * @param  pin      a 3-character string specifying the pin, e.g. "PA7"
 * @param  IOFlag    input or output
 */
void gpio_setIO(char pin[3], uint8_t IOFlag);

/**
 * Reads the specified input pin. Returns 1 or 0.
 * 
 * @param  pin    a 3-character string specifying the pin, e.g. "PA7"
 * 
 * @return  the bit of the input, 1 or 0
 */
uint8_t gpio_read(char pin[3]);

/**
 * Writes to the specified output pin. 
 * 
 * @param  pin    a 3-character string specifying the pin, e.g. "PA7"
 * @param  val    1 or 0
 */
void gpio_write(char pin[3], uint8_t val);

/**
 * Writes the specified data to the specified memory location
 * Helper function for initializing a port
 * 
 * @param  address  the address to write to
 * @param  data     the data to write
 */
void writeToMemory(uint32_t address, uint32_t data);







#endif
