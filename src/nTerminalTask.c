#include "nTerminalTask.h"

#include "task.h"

#include "SerialBridge.h"
#include "nLoadCellTask.h"


static void nTerminalTask( void *pvParameters );


void nTerminalTaskInit()
{
  xTaskCreate(nTerminalTask, "Terminal", 10*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
}

static void nTerminalTask( void *pvParameters )
{
  while(1)
  {
    char c = readCharUART();

    if(c == 't')
    {
      uint32_t th = readHex32UART();
      xQueueSendToBack(cmdQueue, (void*)&th, 0);
    }
  }
}
