#define STM32F207xx

#include "stm32f2xx.h"
#include "FreeRTOS.h"
#include "portmacro.h"

#include "task.h"
#include "queue.h"

#include "SerialBridge.h"
#include "nLoadCellTask.h"
#include "nTerminalTask.h"

// LD2 on PB7
// PB7 AF2 : TIM4_CH2


#define LED_PIN 14
#define LED_ON() GPIOB->BSRR |= (1 << LED_PIN)
#define LED_OFF() GPIOB->BSRR |= (1 << (LED_PIN+16))



static void prvButtonPollTask( void *pvParameters )
{
	int ledOn = 0;
	for( ;; )
	{
		if(ledOn)
		{
			LED_OFF();
			ledOn = 0;
		} else
		{
			LED_ON();
			ledOn = 1;
		}

		vTaskDelay( 2000 / portTICK_PERIOD_MS );
	}
}

int main()
{
	#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)
	#define NVIC_PriorityGroup_4         ((uint32_t)0x300)
  /* Set the PRIGROUP[10:8] bits according to NVIC_PriorityGroup value */
  SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup_4;

	/* Enbale GPIOB clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

	// LED PIN
	GPIOB->MODER |= (1 << (LED_PIN << 1));
	GPIOB->OSPEEDR |= (3 << (LED_PIN << 1));

	HX711_setup();

	/* Enable HX711 Interface */
	// PF_12 -> Data
	// PF_13 -> Clock

	SerialBridgeInit();

	writeStrUART(NEWLINE"--------"NEWLINE);
	writeStrUART("Hello World!"NEWLINE);

	writeHexLineUART(0xF5F5F5F5);


	xTaskCreate( prvButtonPollTask, "Led", 10*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	nLoadCellTaskInit();
	nTerminalTaskInit();

	vTaskStartScheduler();

}

void vApplicationTickHook( void )
{

}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues or
	semaphores. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* Called on each iteration of the idle task.  In this case the idle task
	just enters a low(ish) power mode. */
}
