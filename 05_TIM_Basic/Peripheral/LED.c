#include "stm32f10x.h" // Device header
#include "Delay.h"

void Led_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 變更哪一組的RCC, 開啟/關閉

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Led1_On()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}

void Led2_On()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}

void Led1_Off()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void Led2_Off()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
}

void Led1_Turn()
{
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1) == 0) // 如果亮
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_1); // 關掉
	}
	else // 如果不亮
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_1); // 打開
	}
}

void Led2_Turn()
{
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_2) == 0) // 如果亮
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_2); // 關掉
	}
	else // 如果不亮
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_2); // 打開
	}
}
