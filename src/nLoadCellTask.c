#include "nLoadCellTask.h"

#include "task.h"

#include "SerialBridge.h"


static void nLoadCellTask( void *pvParameters );

static uint32_t threshold = 0;

void nLoadCellTaskInit()
{
  cmdQueue = xQueueCreate(10, sizeof( uint32_t));

	xTaskCreate( nLoadCellTask, "LoadCell", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
}

static void nLoadCellTask( void *pvParameters )
{
  while(1)
  {
    uint32_t cmdBuffer;
    if(xQueueReceive(cmdQueue, (void*) &cmdBuffer, 0))
    {
      threshold = cmdBuffer;
    }

    taskENTER_CRITICAL();
    uint32_t value = HX711_read();
    taskEXIT_CRITICAL();

    if(value > threshold)
    {
      writeCharUART('%');
      writeHexLineUART(value);
    }

    // Sample approx. once every 10ms
    vTaskDelay( 10 / portTICK_PERIOD_MS );
  }
}
