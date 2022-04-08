# LaserHandle
Code allowing to toggle on/off lasers in pre-defined sequence

This program requires, at its minimum, a shift register 74HC595, 2N2222 transistors  and some 4.7k resistors.
And lasers, of course: the current ones are cheap 5v dot, 5mw, 650 nm type.
Even if internal resistors are supplied, a 20 ohm resistor is attached to each negative leg

The circuit has been tested with a Arduino Uno and a STM32 blue pill.  
Arduino nano could handle it as well, but maybe the pinout will need to be revised
The 74HC595 was chosen for its ease of control

A SD card reader is also part of the project, handled via SPI :                                
   SR_LATCH_PIN : ST_CP - pin 12  store register clock pin  -  PB3 = STM32-pin 19  /  uno-pin 3
   SR_CLOCK_PIN : SH_CP - pin 11  shift register clock pin  -  PB4 = STM32-pin 20  /  uno-pin 4
   SR_DATA_PIN  : DS    - pin 14  serial data input         -  PB5 = STM32-pin 21  /  uno-pin 5
                  OE    - pin 13  Output enabled  is connected to ground 
Eventually, the SD card reader cold be replaced by FLASH WinBond 64 megs chips, still using the SPI protocol.
It has a smaller footprint then the SD card, and I suspect it will consume less current while providing faster access to data


