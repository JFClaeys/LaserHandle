# LaserHandle
Code allowing to toggle on/off lasers in pre-defined sequence

This program requires, at its minimum, a shift register 74HC595, 2N2222 transistors  and some 4.7k resistors.
And lasers, of course: the current ones are cheap 5v dot, 5mw, 650 nm type.
Even if internal resistors are supplied, a 20 ohm resistor is attached to each negative leg

The circuit has been tested with a Arduino Uno and a STM32 blue pill.  
Arduino nano could handle it as well, but maybe the pinout will need to be revised
The 74HC595 was chosen for its ease of control
A SD card reader is also part of the project, handled via SPI :                                

Eventually, the SD card reader could/will be replaced by a Flash WinBond 64 megs chips, still using the SPI protocol.
It has a smaller footprint then the SD card, and I suspect it will consume less current while providing faster access to data

The SD / Flash is used to hold sequences of patterns, where each laser will be triggered for a certain amount of time.
Given the size of RAM available on arduino, the sequences will be read on the support periodically.
When using lettering as a sequence, there is an array of patterns defining uppercase alphabet letters that is placed on Arduino's flash memory (see alphas.h)
So when writing a sequence, one would just need to specify the letter and the system will look over the actual pattern.
