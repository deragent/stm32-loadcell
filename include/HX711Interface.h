#ifndef HX711_INTERFACE_H
#define HX711_INTERFACE_H

#define STM32F207xx
#include "stm32f2xx.h"

#include <stddef.h>


// Config
#define HX711_GPIO GPIOF
#define HX711_AHB1ENR RCC_AHB1ENR_GPIOFEN

#define HX711_DATA_PIN 12
#define HX711_CLOCK_PIN 13


// Helper macros
#define HX711_DATA_VALUE() ((uint32_t) (HX711_GPIO->IDR & (1 << HX711_DATA_PIN)) >> HX711_DATA_PIN)
#define HX711_CLOCK_HIGH() HX711_GPIO->BSRR |= (1 << HX711_CLOCK_PIN)
#define HX711_CLOCK_LOW() HX711_GPIO->BSRR |= (1 << (HX711_CLOCK_PIN+16))



void HX711_setup();

void HX711_wait100ns(uint32_t ns_100);

uint32_t HX711_read();

#endif
