#ifndef SERIAL_BRIDGE_H
#define SERIAL_BRIDGE_H

#define STM32F207xx
#include "stm32f2xx.h"
#include <stddef.h>

#include "FreeRTOS.h"
#include "queue.h"

QueueHandle_t recvQueue;
QueueHandle_t sendQueue;

#define NEWLINE "\n\r"

void SerialBridgeInit();

void initDebugUSARTPins();
void initDebugUSART();


void USART3_IRQHandler(void);


void writeCharUART(char c);
void writeUART(char* buf, size_t buflen);
void writeStrUART(char* buf);
void writeHexUART(uint32_t val);
void writeHexLineUART(uint32_t val);

void writeRegUART(char* title, uint32_t val);

char readCharUART();
uint32_t readHex32UART();
void readUART(char* buf, size_t buflen);

#endif
