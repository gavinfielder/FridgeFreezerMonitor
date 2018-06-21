
# Fridge and Freezer Monitoring System

![Results 2](https://i.imgur.com/K6dCP83.jpg?1)

_Room temperature demonstration of prototype during judging._


## Introduction, Motivation

We built a monitoring system for a refrigerator. It monitors whether the door is closed as well as the internal temperature of the fridge, generating an alarm when either the door is left open or the temperature inside the refrigerator goes above a threshold. 

This can be adapted into two categories of product:
 - As a component for manufacturers of home refrigerators.
 - As a standalone system that can be installed on commercial refrigerators. 

In either case, it provides a layer of security against food spoilage by alerting the user when the integrity of the refrigerator system is compromised. 

As this is a reasonably simple device that any company with investment in electrical engineers could produce, the second category of product is more likely to attract customers. 
Related Work

Similar products already exist. Our implementation is neither a new product nor an improvement. Since, ultimately, this device is a security feature, the primary way our product can be competitive in the marketplace is in its reliability. In this way, simplicity is its best feature. 

## Specification

The target user is any company or store that has a refrigerator in which significant stock is stored and does not have a built-in monitoring system. It would likely be sold in two ways: 1) by contacting the business directly and delivering a sales pitch, and 2) providing units to construction companies building new restaurants or stores. For the home refrigerator route, sales would likely be made by bidding contracts to refrigerator manufacturers. 

The minimum specifications for a saleable product is:
 - Can be installed on an existing refrigerator
 - Runs on a reliable power source (AC adapter, or battery system that can detect low battery)
 - Accurately reads temperature within 2 deg F
 - Produces a notification when the fridge door is left open
 - Produces a notification when the internal temperature rises above a threshold

The competitive advantage for this product is made on these factors, in order of importance:
 1. Reliability
 2. Configurability of thresholds
 3. Energy consumption
 4. Price
 5. Fine temperature accuracy

In addition, significant market value is added as well if the unit can function for freezers as well as it can for refrigerators. 

Reliability is a complicated measure. While the entire field of reliability engineering could not be simplified into a simple strategy, simplicity and modularity are key strategies to high reliability. 

Configurability of thresholds can be achieved. 

Energy consumption can be optimized by reducing the clock speed, which is possible because this device should not need a fast clock, and also by reducing the number of ports used, or in general using a simpler microcontroller.

Price can also be reduced by using a simpler microcontroller and by using a thermistor of the minimum viable resolution for the 0 to 60 degree range Fahrenheit, though conversely, fine temperature accuracy is primarily achieved by a higher quality thermistor.
 
### Specifications for the first iteration

For the first minimum viable product, our specifications are as follows:
 - Accurately reads temperature within 3 deg F
 - Produces an alarm when the fridge door is left open
 - Produces an alarm when the internal temperature rises above a threshold

This provides a good starting point for future improvements.

## Design
### Block Diagram

![Block Diagram](https://i.imgur.com/k1LHCSa.png)


### Hardware

#### LCD

In order to give the user feedback regarding the state of the system, we connected a Nokia 5110 LCD to port A (as per Valvano’s driver) of the receiver Tiva board to use to output the current ambient temperature of the thermistor.

#### 7 Segment Display

Because of a problem relating to the system clock, part way through the project development we could no longer get the Nokia LCD to display the appropriate output. As such, we considered the alternative of using two common anode 7 segment displays to display the temperature data instead. We built a test circuit for the displays by connecting each cathode to port B of the Tiva board through 220? resistors, while the common anode pins were each connected to the emitter pin of a BJT transistor. We ran 5V from the Tiva clock bus to the collector pins and we connected two pins on port A to the base pins of the transistors to trigger the switching. We then proceeded to write drivers to control these displays through multiplexing. Ultimately, however, we were able to solve the clock issue and we migrated back to the LCD for the sake simplicity and modularity, should we ever want to add more visual output to the system.

#### Door Switch

The switch used to simulate the door opening and closing is a dpst push button wired with negative logic to the Tiva board.

#### Thermistor

In order to measure temperature, we purchased a negative temperature coefficient thermistor from the IEEE chapter at Chico. Unfortunately, the component did not have a data sheet or any markings to indicate its type or characteristics. We decided to use the component anyways and went to work characterizing it. We determined how its resistance relates to temperature (see ADC and Thermistor Characterization under Development Plan) and connected it in a voltage divider with a reference resistor to generate a predictable voltage, which we collect with the Tiva’s ADC.

##### Considerations for future iterations

Thermocouples provide a significantly faster response time and create a voltage difference to measure without drawing current from the Tiva, though their outputs may need to be amplified. Similarly, thermostats require no current to operate, and could be used without the ADC (since they operate like switches), but only measure a single temperature.

#### DAC and Sound Output

Since we wanted to sound an alarm in the event that the internal refrigerator temperature gets too warm or if the door is left open, we needed a method to produce sound. We did this by using a 5-bit DAC connected to a powered speaker through a conventional 3.5mm jack plug. In order to drop the output voltages we used resistor values of 1.5k?, 3k?, 6k?, 12k?, and 24k?.

### Software

#### Bus frequency

Both boards use PLL at 80 MHz.

##### Considerations for future iterations

Reducing the clock speed will reduce energy consumption.

#### FriendlyWriter and generalized initialization

In order to improve the speed and accuracy of development, we wrote a module called FriendlyWriter that reads and writes memory addresses. It consists of 3 functions, Read, Write, and FriendlyWrite. Each takes a 32-bit unsigned integer address, Write also takes an unsigned integer value to write, and FriendlyWrite also takes an unsigned integer representing a mask such that only the bits of the passed value corresponding to the high bits in the mask will be written to the register, and the bits in the register corresponding to the zero bits of the mask retain their value. With this functionality, we wrote generalized initialization functions for both UART simplex communication and the door switch of the sensor unit.

The way generalized initialization works is by defining base addresses of different modules within constant arrays, and defining the offsets for registers in each module. By passing the number of the desired module, the base address is looked up in an array and then the offset is added to that base address to derive the address of a desired register. It then reads and writes to these dynamically-determined addresses using the FriendlyWriter module.

#### UART Library

The UART initialization function allows us to specify the port and pin number, the UART number, and whether it is Rx or Tx, such that when we wish to switch pins or switch UART modules, we need only change a single line in the main program. By including functions for both Rx and Tx, we also were able to use identical files in both the receiver and the transmitter. The UART library assumes a bus clock frequency of 80 MHz and sets the baud rate at 9600 bits/s.

##### Considerations for future iterations

We could have used the normal path for initialization in which specific registers are hard coded into the initialization. This has the advantage of being fast in initialization as well as easy to read for code maintenance of the initializing functions, but initialization time is not a concern for this product. Instead, generalized initialization allows for faster development and turnaround time, reduces the frequency of programmer error, and much less time is spent bug-fixing when a pin change is deemed necessary. The tradeoff makes it worth implementing generalized initializations for other peripherals, but only when the next big “strange bug” is encountered.

#### Tiva 1 (Sensor unit)

The goal for communication to the interface unit is to send 7 bits of temperature data in Fahrenheit and 1 bit of alarm request. The goal internal to the sensor unit, therefore, is to continually update the status of the fridge monitor with regards to the state of the door and the internal temperature of the fridge.

![Finite state machine model](https://i.imgur.com/KriGl3o.png)


##### Main Program

The software is implemented as a simple state machine with three states: DOOR_CLOSED and DOOR_OPEN, in which the alarm bit is cleared, and DOOR_OPEN_ALARM in which the alarm bit is set, indicating the door has been left open. The main loop of the program handles the current state and checks the conditions for transitioning to its other states, such as whether the door has been opened or closed or whether the door has been open for more than a specified time interval. The way the state machine checks the time elapsed is by reading a counter variable which is incremented by the SysTick periodic interrupt at 5 Hz. At 25 counts threshold, the time from the door being open to the alarm being requested is 5 ± 0.2 seconds.

The state machine uses two LEDs onboard the Tiva. If the state machine reads the door as open, it turns on the red LED (PF1). This is intended as a user interface on the fridge itself, so that the user can see when the door is reading as open or closed. If the state machine is setting the alarm bit, it turns on the green LED (PF3). We included this as debug functionality.

Outside of the state machine framework, the main loop checks if the ADC has new data from the thermistor, converts the ADC sample to Fahrenheit if so, using a fixed-point format according to a conversion function determined by characteristic data.

Next, if the most recent calculation of temperature in Fahrenheit is above the configured threshold, the alarm bit is set regardless of door state.

Lastly, the main loop sends both the most recent temperature data and the alarm bit to the UART module to be communicated.

##### Thermistor

Since the thermistor is an analog device, we must use one of the Tiva’s onboard ADC units. To set this up, we initialize ADC0 to channel Ain1 on pin PE2, and then we initialize SysTick with periodic interrupts at 5 Hz. Every SysTick interrupt request, we take an ADC reading. When the ADC is finished, it stores the result in a global variable to be retrieved by the main program, and sets a flag that new data is available.

While the ADC is reading, we turn on the onboard blue LED (PF2) to enable heartbeat debugging. Since the ADC takes some appreciable time and the frequency is low enough to be visible to human vision, we can then ensure that the ADC is generating input by visibly seeing the blue LED flicker at some low duty cycle.

SysTick also increments a global counter variable. We use this as a general purpose timer.

##### Temperature Conversion

For demonstration purposes, we implemented a linear fit between ADC sample and degrees farenheit on the range of 52°F to 90°F.

The table below is a subset of the linear fit data. For the full data set, see ADC and Thermistor Characterization under Development Plan.

![Table 1](https://i.imgur.com/ymNDbwK.png)

Here voltage is the voltage calculated by the voltage divider equation with a resistor we tested at 981.5 ?. For more information, see the full data set and procedure in ADC and Thermistor Characterization under Development Plan.

##### UART Transmission
For a passive temperature sensor, the accuracy of the reading at the immediate time is of more importance than every single reading being sent. For this reason, we decided our transmit function to only transmit data if the UART is currently idle. The way it does this is upon being called by the main program (and accepting temperature data and alarm request), it checks if the transmit FIFO is empty, and if so, it formulates a unsigned 8-bit integer with the lower 7 bits temperature in Fahrenheit and the most significant bit, and then writes this data package to the FIFO. If the FIFO is not empty, then the previous data has not yet been sent, and so it exits, discarding the data. Since the bus speed runs at 80 MHz and the frequency of new ADC readings is only 5 Hz, the same data is inevitably repeatedly sent anyway even in this lazy transmission model. But more importantly, by sending only the most recent data, there is less time lag between the reading of the data by the sensor unit and display of the data by the interface unit.

![UART Tx flowchart](https://i.imgur.com/M3nlo3r.png)

##### Door Switch

The door is a simple negative logic switch--if the door is closed, the switch is closed, so a raw data reading of 1 implies the door is closed and a reading of 0 implies the door is open. When the switch state changes, an interrupt is generated which sets a global doorStateChanged variable. When this flag is set, the main program knows to call a function which reads the current state of the door switch, via a function which returns 1 if the door is open and 0 otherwise. By calling this read function, the flag is also reset.

##### Total software flowchart for sensor unit

![Total software flowchart for sensor unit](https://i.imgur.com/6fTRlGE.png)

##### Considerations for future iterations

While the main program could be implemented in ways other than a state machine, it would inevitably have to use several conditional statements that approach the same functionality, and so representing the problem as a state machine clarifies the issue and simplifies the project.

In future iterations, the green LED debug function for the door open alarm state can be taken out.

The decision for SysTick to have double-duty in both triggering ADC samples and acting as a general purpose timer for the open door alarm state violates the single responsibility principle in good software design and can introduce problems if, for example, the ADC sampling rate is changed. By only using one timer, however, this reduces energy consumption and simplifies the project in the sense that it reduces likelihood of programmer error in total system integration.

The largest limitation of this implementation is the fact that the temperature is an unsigned value, and so cannot handle values below 0°F. If temperatures below 0°F are needed, one possible implementation that uses 7 bits as currently is using a biased temperature, i.e., set 0 to correspond to -50°F and the max value of 127 shall then refer to 77°F and above, which is clearly outside the refrigerator functioning range and should therefore be more than sufficient to handle both refrigerators and freezers.

The linear fit we used is only appropriate in the room temperature range, and the thermistor characterization has nonlinear resistance with temperature especially outside of room temperature range, and so for future iterations the temperature conversion needs to be changed from a linear calculation to a 2nd- or 3rd-order polynomial at least. The Steinhart test mentioned in ADC and Thermistor Characterization under Development Plan, while potentially accurate, uses a logarithmic function, which has the added complexity of needing double precision data types in order to use the math.h standard library functions. As our microcontroller was hard-faulting on any attempt to use double type, we abandoned this approach in favor of implementing a simple linear model for demonstration and proof of concept. Should the double implementation be refactored and made to work, however, it could produce accurate results.

The decision to put the alarm request and the temperature data in the same frame has both advantages and drawbacks. The drawback is that it’s a less readily understood by any developer working with the interfacing device. The advantage is that every UART frame means the same thing. One alternative, by contrast, would be to send the alarm request in one frame and then the temperature in another frame, but this would drastically increase the complexity of communication. If there’s one clear viable alternative, it is that the alarm status might not be determined by the sensor unit, but instead by the interface unit. This reduces the sensor unit to have the single responsibility of collecting and transmitting data. The complication in this alternative is that the sensor unit would also have to transmit the door open or closed state in order for the time-based alarm for the door being left open to be determined by the interface unit, so the issue of multiple data needing to be sent per frame would not actually be solved, but this alternative may still be preferable as it allows the interface to be the sole point of configurability of thresholds when this feature is implemented, while maintaining the simplicity of simplex communication.

The main loop currently maintains its state for many cycles before changing to a new state, and repeat data is sent over the communication wire, which is not energy efficient. A change that should be made in future iterations is moving to a completely interrupt-based system using SysTick periodic interrupts to take the ADC sample, do any processing, and transmit the data. This will enable the interstice to run in a low-power wait mode.

### Tiva 2 (Interface unit)

#### Main Program

The main program busy waits on a function reading the UART Rx FIFO empty flag to detect when new data is available.

When new data is available, it is read and processed.

 - The alarm request bit (bit 7) is read and the alarm is turned on or off accordingly.
 - The temperature data (bits 0-6)  is written to the LCD as a decimal number with a degF unit specifier.

![enter image description here](https://i.imgur.com/Dkj8W2F.png)

#### Alarm

The alarm is produced by a 5-bit DAC outputting an approximated sine wave to an audio device. It does this by iteratively outputting values to the DAC from a table. SysTick periodic interrupts trigger each new value output to the DAC.

The table consists of 128 values from 0-31 approximating one period of a sine wave. New values are output at a frequency of 56.320 KHz. Therefore the frequency of the sine wave output is 440 Hz.

When AlarmOn is called, the SysTick reload value is reset to 1419, producing the 56.320 KHz. The outmask is also reset to allow all DAC bits to pass through.

When AlarmOff is called, the maximum reload value for SysTick is set and the outmask is set to 0. This forces each DAC output to be zero.

![DAC flowchart](https://i.imgur.com/dz7MSHE.png)

#### LCD

The LCD uses the supplied Nokia 5110 library.

#### Considerations for future iterations

Since the main loop only busy waits on UART communication, this can be worked to be interrupt-driven for much lower energy consumption.

## Development Plan

### Planned Development

#### Acquire parts

All parts not provided in class (such as the Nokia LCD, 3.5mm jack plug, and resistor network necessary to create a DAC) were purchased from the Chico IEEE chapter. This includes the thermistor, BJT transistors for the 7 segment display multiplexer, and various resistors used throughout the project.

#### Thermistor circuit

In order to use the thermistor as a temperature sensor, we could have used two primary circuits; either we could use the variable resistive properties of the thermistor to short a circuit path to trigger an input when temperature exceeded a predetermined value (since the thermistor resistance decreases with rising temperature), or we could use a voltage divider to generate a predictable voltage in a range as the temperature changes. The latter involves using the Tiva’s ADC to collect data rather than simply using a “boolean” GPIO input pin, but allows us to give the user more feedback regarding the system.

#### 7 segment display circuit

Since the temperatures we sought to display were all two digit decimal values (though an actual installed unit could manage with one digit numbers if measuring in Celsius), we needed a way to drive at least two 7 segment displays. Instead using at least 14 pins on the Tiva board, it made more sense to only use 9 and multiplex the displays, that is alternate displaying data on one, then the other, at a rate faster than humans can perceive, such that both displays appear to be on. In order to do this, the selection pins on each of the 7 segment displays are tied together and sent data. Then the common anode of one display is sent a signal while the anode of the other display is pulled to ground. This anode switching behavior is accomplished using two BJT transistors (though MOSFETS have better switching performance, I believe it would be negligible in this context). The transistors are just toggled between the on and off states (it should not matter if the transistor is in the saturation region or the forward active region since all of the collector current will be directed to the emitter path in the circuit) to power each LED array one at a time.

#### ADC and Thermistor Characterization

![Characterization processes](https://i.imgur.com/hfiCGIh.jpg)

We did not have a data sheet for the thermistor we used. In order to characterize the thermistor, we used a beaker of water and a thermometer, and starting from ice water, took periodic measurements as the water warmed. Once the water reached room temperature (about 25°C), the beaker was emptied and refilled with boiling water. We waited for the temperature to lower well below 100°C to protect the insulation (hot glue, with a melting point near 120°C) that kept the thermistor from shorting in the water. Measurements were then periodically taken until the water settled at room temperature once again.

![All thermistor characterization data](https://i.imgur.com/sXX1nSi.png)

This data was analyzed on a spreadsheet program, but the best-fit curve approximation was too high of an order to be useful. Instead we used a common thermistor equation, a simplified form of the Steinhart-Hart equation 

![Steinhart-Hart equation](https://i.imgur.com/pGP3VOk.png)

, where _A, B, C_ are experimentally determined coefficients that tend to be small and require high precision. This relationship gave a much closer approximation at a considerably lower order. Unfortunately, we encountered errors dealing with the doubles needed to store the Steinhart coefficients, so we instead took a small operating region and created a linear model to use for ADC conversion. The following MatLab plot demonstrates the accuracy of the various approximations.

![Thermistor characterization](https://i.imgur.com/nLJTyby.png)


#### Write software

Since the software uses many parts from previous lab assignments, much of the peripheral code was recycled from previous projects in the first version of the software. The ADC module was kept identical as we did not switch the pin or any options in initialization other than the sample frequency in setting up SysTick interrupts.

While we used previous project files as a base for each unit, the main program in each was written from scratch.

While some UART libraries were provided, we opted to write our own UART functions, especially as we were sending specialized packages of data rather than generic ascii data. Our design also calls for only sending data when the UART is idle, which is unsupported by the provided UART library. Our first choice for the UART modules were UART3 for both boards, PC7 on the sensor unit and PC6 on the interface unit. Both of these had to change before all the hard fault issues were resolved.

Apart from serious bugs in system integration, all the software modules operated, independently, as expected reasonably quickly. Some defects were injected into the code, such as:
 - Forgetting to clear the interrupt flag of the door switch
 - Testing a masked value against 1 instead of 1 shifted

These errors were caught during simulator debugging before causing any significant issue.

### Debugging

#### Bug-fixing ADC and temperature sensing

The steinhart test module, using a natural log, required, in its simplest possible implementation, using double precision floating point data type. We attempted implementing it this way, but every time the software ran on the board it would hard fault the first time it entered the temperature conversion function. Hypothesizing that double floating point type was the cause of the hard fault, we decided to postpone implementation of the Steinhart test model and try to find a linear model for demonstration purposes.

For demonstration, it was assumed we would be testing the device under room temperature conditions, and so we found a linear fit on the range of 52°F to 90°F using the LINEST function of google sheets, comparing measured degrees Fahrenheit versus theoretical ADC sample based on measured resistance and computed voltage across a voltage divider. Of the 39 data points we had in this range, only one conversion had an error greater than 3% between the measured temperature and the modelled temperature, and we believe this is due to measurement error.

The LINEST function returned the line

degF = (0.05669595055)*ADC + (-34.0350707)

To rewrite the conversion using fixed-point format, we decided on a binary fixed point precision of 16 bits, because both the slope and the range of inputs is small. This resulted in the following conversion:

degF = ((3716)*ADC + (-2230522)) / 65536

We believe this model accurately reads temperatures in the room temperature range.

#### General bug-fixing

Since one of our most common errors were unexplained hard faults, and that fixing this error commonly required switching pins of different modules, we decided that in order to speed up development we needed generalized initialization such that changing the pin or port of a peripheral only needed one line of code in the main program rather than changing the entire implementation file to accommodate the hardware change. To facilitate this, we first wrote a module called FriendlyWriter which reads and writes data to a provided address. Using this, we implemented dynamic address lookup with base addresses and register offsets and wrote logic that navigates the particular setup of the Tiva board--for example, in the door switch generalized initialization, if port F is selected, it activates interrupt 30 in the NVIC, but if anything else is selected, it can be linearly determined from port A at interrupt 0, port B at interrupt 1, etc.

This had positive results. Our initial selection for the door switch was PA2,  but after this caused problems with the ADC initialization, we switched to PF0 relatively quickly. Since the UART initialization was generalized, we could import the new library into the interface unit and immediately use it. Thus with a single line of code, we switched from using UART3 Rx on PC6 to UART1 Rx on PB0, which resolved another hard fault integration bug.

We also found that hard faults were being caused by, or, at least, being resolved by, switching the order of initialization. For example, the last hard fault bug in the interface unit was resolved by moving the Nokia LCD initialization and the UART initialization to before the PLL initialization.

At one point, we were reading strange values from the UART communication before realizing there was not a common ground at that point. Upon reconnecting the common ground, UART worked as expected.

### First Iteration results

![Results 1](https://i.imgur.com/GpXwfIB.jpg)

![Results 2](https://i.imgur.com/K6dCP83.jpg?1)

## Conclusions

The fact that our major issues were caused by internal behavior of the Tiva rather than problems we injected into the product ourselves emphasizes the need for fail-fast development and it highlights the value of tools that can enable rapid turnaround upon failure and assist in debugging. It also highlights that modules sharing clocks and other features of the board must represent a common problem in developing embedded systems. 

Other than this, the only new thing about this project for us was UART communication, which we didn’t have any problems with directly. 

The first improvement of this product is a more appropriate thermistor that more accurately reads temperature at the -10°F to 50°F range and with less noise. Using a better thermistor, we could then accurately characterize it using a polynomial model, or by finding a way to implement the Steinhart test model to calculate temperature. Related to this, by taking the temperature data to a bias, we can represent negative values. 

The next most useful change to make to this product in order to bring it to salable status is configurability of thresholds. This might best be accomplished by moving the threshold comparison to the interface unit and have the interface unit accept configuration by the user. This would mean the data package can be 7 bits of temperature data and 1 bit of door status instead of the high bit being an alarm request, and doing it this way maintains simplex communication. 

Finally, we would decide on a reliable power source as well as a housing for the sensor unit. 

Optionally, getting the 7 segment display to work might be better than the LCD because it would entail lower energy consumption. It might interfere with how configurability is implemented for the user, though.

## Credits and Acknowledgments

 - Hardware
   - Acquiring parts - Michael Musick
   - Designing circuitry - Michael Musick
   - Assembling circuitry - Michael Musick
 - Temperature sensing
   - Temperature sensor development planning - Michael Musick
   - ADC and thermistor characterization measurements - Michael Musick and Gavin Fielder
   - ADC sample conversion, Steinhart test model - Michael Musick
   - ADC sample conversion, linear model for demonstration - Gavin Fielder
 - Writing software
   - Program flow design - Gavin Fielder
   - Sensor unit main - Gavin Fielder
   - Interface unit main - Gavin Fielder
   - ADC driver - Michael Musick
   - DAC alarm driver - Gavin Fielder
   - Door switch driver - Gavin Fielder
   - Sensor unit UART driver, first attempt  - Gavin Fielder
   - Interface unit UART driver, first attempt - Gavin Fielder
   - FriendlyWriter module - Gavin Fielder
   - Generalized initialization - Gavin Fielder
   - 7-Segment display driver - Michael Musick
 - Debugging
   - System integration refactoring and general debugging - Michael Musick and Gavin Fielder
   - Dr. Mustafa for various advice
 - Documentation
   - Introduction, Motivation - Gavin Fielder
   - Related Work - Gavin Fielder
   - Specification - Gavin Fielder
   - Design
      - Hardware section - Michael Musick
      - Software section - Gavin Fielder
   - Development plan
      - Acquire parts section - Michael Musick
      - Design circuitry section - Michael Musick
      - ADC and Thermistor Characterization section - Michael Musick
      - Write Software section - Gavin Fielder
      - 7-Segment display section - Michael Musick
      - Bug-fixing ADC and temperature sensing section - Gavin Fielder
      - General bug-fixing section - Gavin Fielder
   - Conclusions - Michael Musick and Gavin Fielder
   - Credits and Acknowledgements - Gavin Fielder

