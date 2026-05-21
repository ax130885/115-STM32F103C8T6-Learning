#include "stm32f10x.h"

/**
 * @brief  微秒級延遲
 * @param  xus 延遲時長，範圍：0~233015
 * @retval 無
 */
void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;	// 設定定時器重載值
	SysTick->VAL = 0x00;		// 清空當前計數值
	SysTick->CTRL = 0x00000005; // 設定時鐘源為HCLK，啟動定時器
	while (!(SysTick->CTRL & 0x00010000))
		;						// 等待計數到0
	SysTick->CTRL = 0x00000004; // 關閉定時器
}

/**
 * @brief  毫秒級延遲
 * @param  xms 延遲時長，範圍：0~4294967295
 * @retval 無
 */
void Delay_ms(uint32_t xms)
{
	while (xms--)
	{
		Delay_us(1000);
	}
}

/**
 * @brief  秒級延遲
 * @param  xs 延遲時長，範圍：0~4294967295
 * @retval 無
 */
void Delay_s(uint32_t xs)
{
	while (xs--)
	{
		Delay_ms(1000);
	}
}
