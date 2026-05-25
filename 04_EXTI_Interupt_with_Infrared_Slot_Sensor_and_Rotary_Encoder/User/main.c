#include "stm32f10x.h" // Device header
#include "Delay.h"

#ifdef USE_STDPERIPH_DRIVER
#include "stm32f10x_conf.h"
#endif

#include "Infrared_Slot_Sensor.h"
#include "Rotary_Encoder.h"
#include "OLED.h"

volatile uint32_t count = 0;

int main(void)
{

	OLED_Init();
	OLED_Clear();

	Infrared_Slot_Sensor_Init();
	Rotary_Encoder_Init();

	while (1)
	{
		OLED_ShowNum(1, 1, count, 4); // 溢出會在前方填0
		Delay_ms(50);
	}
}
