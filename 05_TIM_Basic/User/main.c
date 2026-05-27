#include "stm32f10x.h" // Device header
#include "Delay.h"

#ifdef USE_STDPERIPH_DRIVER
#include "stm32f10x_conf.h"
#endif

#include "Timer.h"
#include "OLED.h"

volatile uint32_t count = 0;

int main(void)
{

	Timer_Init();
	OLED_Init();
	OLED_Clear();

	while (1)
	{
		OLED_ShowNum(1, 1, count, 4);				 // 溢出會在前方填0
		OLED_ShowNum(2, 1, TIM_GetCounter(TIM2), 8); // Period=10000, Prescaler=7200，所以會以 72M/7200 = 10000Hz 來數數，數到10000歸零，並且觸發中斷。
		OLED_ShowNum(3, 1, TIM_GetCounter(TIM1), 1); // 遮斷兩次紅外線 count = 0
		Delay_ms(50);
	}
}
