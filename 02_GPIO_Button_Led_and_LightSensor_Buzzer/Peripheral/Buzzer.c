#include "stm32f10x.h"

void Buzzer_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitSturct;
	GPIO_InitSturct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitSturct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitSturct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitSturct);
}

void Buzzer_On()
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
}

void Buzzer_Off()
{
	GPIO_SetBits(GPIOB, GPIO_Pin_11);
}

void Buzzer_Turn()
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11))
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_11);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_11);
	}
}
