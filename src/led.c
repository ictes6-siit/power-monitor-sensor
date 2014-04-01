#include "led.h"
#include "stm32f4xx_gpio.h"

void ledInit()
{
  GPIO_InitTypeDef gpio_init_struct;	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  // Configure PD12 to digital output (01), push-pull (0), 50-MHz (10), pull-up (01)
  gpio_init_struct.GPIO_Mode = GPIO_Mode_OUT;
  gpio_init_struct.GPIO_OType = GPIO_OType_PP;
  gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init_struct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_Init(GPIOD, &gpio_init_struct);
}

void ledOn(enum LED Color)
{
	if(Color == LED_ORANGE)
	{
		 GPIO_WriteBit(GPIOD, GPIO_Pin_13, Bit_SET);
	}
	else if(Color == LED_GREEN)
	{
		 GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_SET);		
	}
	else if(Color == LED_RED)
	{
		 GPIO_WriteBit(GPIOD, GPIO_Pin_14, Bit_SET);		
	}		
	else // Blue
	{
		 GPIO_WriteBit(GPIOD, GPIO_Pin_15, Bit_SET);		
	}
}

void ledOff(enum LED Color)
{
	if(Color == LED_ORANGE)
	{
		GPIO_WriteBit(GPIOD, GPIO_Pin_13, Bit_RESET);
	}
	else if(Color == LED_GREEN)
	{
		GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_RESET);
	}
	else if(Color == LED_RED)
	{
		GPIO_WriteBit(GPIOD, GPIO_Pin_14, Bit_RESET);
	}		
	else // Blue
	{
		GPIO_WriteBit(GPIOD, GPIO_Pin_15, Bit_RESET);
	}
}
