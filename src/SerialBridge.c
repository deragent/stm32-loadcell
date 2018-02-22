#include "SerialBridge.h"

#include "task.h"

static void nSerialSendTask( void *pvParameters );


void SerialBridgeInit()
{
  recvQueue = xQueueCreate(256, sizeof(char));
  sendQueue = xQueueCreate(64, sizeof(char));

  initDebugUSARTPins();
  initDebugUSART();

  xTaskCreate(nSerialSendTask, "SerialSend", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
}


void initDebugUSARTPins()
{
	// UART PINs
	// Configure GPIOD pin 8 and 9 as AF7
	GPIOD->MODER |= (2 << (8 << 1)) | (2 << (9 << 1));
	GPIOD->OSPEEDR |= (3 << (8  << 1)) | (3 << (9 << 1));
	GPIOD->AFR[1] |= (7 << ((8-8) << 2)) | (7 << ((9-8) << 2));
}

void initDebugUSART()
{
  	// USART3
  	// TX: PD8
  	// RX: PD9
  	//
  	// 9600 Baud
  	// 8N1

  	// Enable USART
  	USART3->CR1 |= USART_CR1_UE;

  	// 8 Data bits
  	USART3->CR1 &= ~USART_CR1_M;

  	// Set 1 Stop Bit (00 in CR2_STOP)
  	USART3->CR2 &= ~USART_CR2_STOP;

  	// Set Baud Rate to 115200
  	// f_CK = 30 MHz
  	// f_BAUD = 115.2k
  	// UARTDIV = 16.27604
  	USART3->BRR = (uint32_t)(16 << 4) | 4;

  	// IS this next line really needed?
  	USART3->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE;

    NVIC_EnableIRQ(USART3_IRQn);
}


void USART3_IRQHandler(void)
{
  if(USART3->SR & USART_SR_RXNE)
  {
    xQueueSendToBack(recvQueue, (void*)&(USART3->DR), 0);
  }
}



static void nSerialSendTask( void *pvParameters )
{
  while(1)
  {
    char charBuffer;
    BaseType_t rec = xQueueReceive(sendQueue, (void*) &charBuffer, portMAX_DELAY);
    if(rec == pdTRUE)
    {
      while(!(USART3->SR & USART_SR_TXE));
      USART3->DR = charBuffer;
    }
  }
}



void writeCharUART(char c)
{
		xQueueSendToBack(sendQueue, &c, 0);
}

void writeStrUART(char* buf)
{
	// Buffer overflow!!!!! BAD, BAD, BAD
	while(*buf != 0)
	{
		writeCharUART(*buf);
		buf++;
	}
}

void writeUART(char* buf, size_t buflen)
{
	for(size_t bb = 0; bb < buflen; bb++)
	{
    writeCharUART(buf[bb]);
	}
}


void writeRegUART(char* title, uint32_t val)
{
	writeStrUART(title);
	writeStrUART(": ");
	writeHexLineUART(val);
}

void writeHexLineUART(uint32_t val)
{
	writeHexUART(val);
	writeStrUART(NEWLINE);
}

void writeHexUART(uint32_t val)
{
  writeUART("0x", 2);
  for(size_t nn = 1; nn <= 8; nn = nn+1)
  {
    uint8_t nib = ((val >> ((8-nn) << 2)) & 0x0000000F);
    char c = '0'+ nib;
    if(nib > 9)
    {
      c = 'A' + nib-10;
    }
    writeCharUART(c);
  }
}

char readCharUART()
{
  while(1)
  {
    char charBuffer;
    if(xQueueReceive(recvQueue, (void*) &charBuffer, portMAX_DELAY))
    {
      return charBuffer;
    }
  }
}

uint32_t readHex32UART()
{
  uint32_t value = 0;
  for(int i=0; i<8; i++)
  {
    char c = readCharUART();
    if(c >= '0' && c <= '9')
    {
      value <<= 4;
      value += c - '0';
    } else if(c >= 'A' && c <= 'F')
    {
      value <<= 4;
      value += c - 'A' + 10;
    } else if(c >= 'a' && c <= 'f')
    {
      value <<= 4;
      value += c - 'a' + 10;
    } else
    {
      break;
    }
  }
  return value;
}

void readUART(char* buf, size_t buflen)
{
	for(size_t bb = 0; bb < buflen; bb++)
	{
    buf[bb] = readCharUART();
	}
}
