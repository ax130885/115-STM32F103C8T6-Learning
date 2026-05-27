#include "stm32f10x.h" // Device header
#include "Delay.h"

/**
 * @brief 上拉觸發 按鈕接地
 */
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
 * @brief
 * 偵測按鈕放開瞬間的 edge
 *
 * @retval
 * 0: 未按下按鈕。
 * 1: 按下按鈕，放開才回傳1。
 */
uint8_t Key1_Read(void)
{
	uint8_t num = 0;
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0) // 按下
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0) // 按住
		{
			num = 1;
		};
		Delay_ms(20); // 放開
	}
	return num;
}

uint8_t Key2_Read(void)
{
	uint8_t num = 0;
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0) // 按下
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0) // 按住
		{
			num = 1;
		};
		Delay_ms(20); // 放開
	}
	return num;
}
