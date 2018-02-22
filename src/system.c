#define STM32F207xx

#include "stm32f2xx.h"

#define RCC_PLLCFGR_PLLQ_OFFSET 24
#define RCC_PLLCFGR_PLLP_OFFSET 16
#define RCC_PLLCFGR_PLLN_OFFSET 6
#define RCC_PLLCFGR_PLLM_OFFSET 0

void SystemInitError(uint8_t error_source);

#define SYSTEM_INIT_ERROR_FLASH 0x01
#define SYSTEM_INIT_ERROR_PLL 0x02
#define SYSTEM_INIT_ERROR_CLKSRC 0x04
#define SYSTEM_INIT_ERROR_HSI 0x08

void SystemInit() {
	/* Enable Power Control clock */
	RCC->APB1ENR |= RCC_APB1LPENR_PWRLPEN;

	/* Wait until HSI ready */
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);

	/* Disable main PLL */
	RCC->CR &= ~(RCC_CR_PLLON);
	/* Wait until PLL ready (disabled) */
	while ((RCC->CR & RCC_CR_PLLRDY) != 0);

	// Configure PLL
	// Input: HSI (16MHz)
	// f_VCO = f_IN * N / M = 16 * 240 / 16 = 240 MHz
	// f_OUT = f_VCO / P = 240 / 2 = 120 MHz
	// f_SDIO = f_VCO / Q = 250 / 5 = 48 MHz
 	RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSI
							 | (uint32_t)(5 << RCC_PLLCFGR_PLLQ_OFFSET)
							 | (uint32_t)(0 << RCC_PLLCFGR_PLLP_OFFSET)
							 | (uint32_t)(240 << RCC_PLLCFGR_PLLN_OFFSET)
							 | (uint32_t)(16 << RCC_PLLCFGR_PLLM_OFFSET);

	/* PLL On */
	RCC->CR |= RCC_CR_PLLON;
	/* Wait until PLL is locked */
	while ((RCC->CR & RCC_CR_PLLRDY) == 0);

	/*
	 * FLASH configuration block
	 * enable instruction cache
	 * enable prefetch
	 * set latency to 2WS (3 CPU cycles)
	 */
	FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_3WS;

	/* Check flash latency */
	if ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_3WS) {
		SystemInitError(SYSTEM_INIT_ERROR_FLASH);
	}

	/* Set clock source to PLL */
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	/* Check clock source */
	while ((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);

	// APB1: Max f = 30 MHz
	// APB2: Max f = 60 MHz

	// Set AHB1 clock frequency to 120 MHz: prescaler (DIV1)
	RCC->CFGR &= ~(RCC_CFGR_HPRE);

	// Set APB1 clock frequency to 30 MHz: prescaler DIV4 */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

	// Set APB2 clock frequency to 60 MHz: prescaler DIV2 */
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
}

void SystemInitError(uint8_t error_source) {
	while(1);
}
