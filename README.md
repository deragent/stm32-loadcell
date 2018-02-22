# STM32-Loadcell

This project implements the readout of a loadcell via a HX711 ADC. The firmware is implemented using FreeRTOS as a real time kernel. It periodically reads out a value from the HX711 and transmits it over the serial port.

## Resources
- **FreeRTOS/** Contains the basic FreeRTOS kernel and port to the STM32F207
- **include/** All .h files 
- **src/** All .c files
- **openocd/** Config for flashing the STM32F207 using openocd

## Dependencies
- **arm-none-eabi-gcc** GCC compiler for the STM32F207 ARM cpu
- **make** For compilation using the Makefile
- **openocd** For flashing the STM32F207

A version of the **STM-Cube** needs to be placed above the project directory, according to the include path defined in the Makefile.
