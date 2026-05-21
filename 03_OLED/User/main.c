#include "stm32f10x.h" // Device header
#include "Delay.h"

#ifdef USE_STDPERIPH_DRIVER
#include "stm32f10x_conf.h"
#endif

#include "OLED.h"

int main(void)
{

	OLED_Init();
	OLED_Clear();
	
	OLED_ShowChar(1, 1, 'A');
	OLED_ShowString(1, 3, "Hello World!"); // 可用來清除部分字符
	OLED_ShowNum(2, 1, 48763, 6);		   // 溢出會在前方填0
	OLED_ShowSignedNum(2, 7, -48763, 5);
	OLED_ShowHexNum(3, 1, 0xABCD, 4);
	OLED_ShowBinNum(4, 1, 0xABCD, 16);     // 螢幕最大允許 (4, 16) 個字


	while (1)
	{

		Delay_ms(50);
	}
}
