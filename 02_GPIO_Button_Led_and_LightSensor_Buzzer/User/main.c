#include "stm32f10x.h" // Device header
#include "Delay.h"

#ifdef USE_STDPERIPH_DRIVER
#include "stm32f10x_conf.h"
#endif

#include "LED.h"
#include "Key.h"
#include "Buzzer.h"
#include "LightSensor.h"

// 使用標準外設庫實作
int main(void)
{

	Led_Init();
	Key_Init();
	Buzzer_Init();
	Light_Sensor_Init();

	Led1_Off();
	Led2_Off();
	Buzzer_Off();

	while (1)
	{
		// 按鈕放開時 切換燈的狀態
		if (Key1_Read())
		{
			Led1_Turn();
		}

		if (Key2_Read())
		{
			Led2_Turn();
		}

		// 遮住才響
		if (Light_Sensor_Read() == 1)
		{
			Buzzer_On();
		}
		else
		{
			Buzzer_Off();
		}

		Delay_ms(50);
	}
}
