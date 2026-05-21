#include "stm32f10x.h"
#include "Delay.h"

void Light_Sensor_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
 * @retval
 * 照到光: 0
 * 遮住光: 1
 */

uint8_t Light_Sensor_Read()
{
	return (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) != 0);
}
