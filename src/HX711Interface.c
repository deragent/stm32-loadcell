#include "HX711Interface.h"

void HX711_setup()
{
  // Enable the
	RCC->AHB1ENR |= HX711_AHB1ENR;

	HX711_GPIO->MODER |= (1 << (HX711_CLOCK_PIN << 1));

	HX711_GPIO->OSPEEDR |= (1 << (HX711_CLOCK_PIN << 1));


	// Enable Timer 4 for precise wait
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	TIM4->CR1 |= TIM_CR1_ARPE | TIM_CR1_DIR;
	// Prescale by 1 to get 10MHz clock
	TIM4->PSC = 3;


  // Set the clock pin high (IDLE)
	HX711_CLOCK_HIGH();
}

void HX711_wait100ns(uint32_t ns_100)
{
	TIM4->ARR = ns_100;
	TIM4->EGR |= TIM_EGR_UG;

	TIM4->SR &= ~TIM_SR_UIF;
	TIM4->CR1 |= TIM_CR1_CEN;

	while(!(TIM4->SR & TIM_SR_UIF));

	TIM4->CR1 &= ~TIM_CR1_CEN;
	TIM4->SR &= ~TIM_SR_UIF;
}


uint32_t HX711_read()
{
	uint32_t buffer;
  buffer = 0;

	HX711_CLOCK_LOW();

  while (HX711_DATA_VALUE() == 1);

	HX711_wait100ns(3);

  for (uint8_t i = 0; i < 24; i++)
  {
  	HX711_CLOCK_HIGH();

		HX711_wait100ns(3);

    buffer <<= 1 ;
		buffer += HX711_DATA_VALUE();

		HX711_CLOCK_LOW();
		HX711_wait100ns(3);
  }

  for (uint8_t i = 0; i < 1; i++)
  {
  	HX711_CLOCK_HIGH();
		HX711_wait100ns(3);
		HX711_CLOCK_LOW();
		HX711_wait100ns(3);
  }

	buffer = buffer ^ 0x800000;

  return buffer;
}
