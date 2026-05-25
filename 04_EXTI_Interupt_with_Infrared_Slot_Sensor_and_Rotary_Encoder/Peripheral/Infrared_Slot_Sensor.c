#include "stm32f10x.h"

void Infrared_Slot_Sensor_Init(void)
{
	// 開啟電源時脈
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// EXTI 在外設，預測開啟。無須開啟RCC。
	// NVIC 在內核，預設開啟。無須開啟RCC。

	// 初始化 GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // RM page 113, Table31: EXTI的GPIO配置須為 浮空輸入/上拉輸入/下拉輸入 任一
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// 配置 GPIO 端口到 EXTI
	// RM Page 137:
	// GPIO 有 ABC 三組，每組共 16 個Pin。
	// 同編號的 GPIO Pin 只能映射一個到 EXTI 上。
	// 例如以下兩個只能擇一
	// GPIO_EXTILineConfig(GPIO_PortSourceGPIO"A", GPIO_PinSource11);
	// GPIO_EXTILineConfig(GPIO_PortSourceGPIO"B", GPIO_PinSource11);
	// 小心: GPIO_PinSource 不可以用 |
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);

	// 初始化 EXTI
	// RM Page 137: EXTI 的中斷來源共 20 個線路, 16 組為 GPIO, 另外四組分別為 PVD(電壓檢測), RTC(時鐘定時), USB, ETH(乙太網喚醒)
	// RM Page 132, Table 55: 部分 EXTI 會共用一個中斷函數 (IVT 地址相同, 接到同一個 NVIC 接口上)
	// EXTI0,  EXTI1,  EXTI2,  EXTI3,  EXTI4,  EXTI[5-9],  EXTI[10-15]
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line11;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	 // 中斷:透過NVIC CPU會暫停工作以執行中斷函數; 事件:產生的脈衝訊號不會進入主程式、NVIC與CPU，而是直接由另一個硬體偵測。
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // 遮斷時 1，未遮斷時 0。所以下降緣觸發 相當於 遮斷結束時觸發中斷。
	EXTI_Init(&EXTI_InitStruct);

	// 設置中斷優先級分組 (整個硬體共通，跨檔案/感測器須確保設置一致!!!)
	// 總共 4 bits, 要分配給搶占優先級&響應優先級分別個幾 bits。
	// 搶占優先級: 中斷當前函式，優先執行中斷函數。
	// 響應優先級: 當前函式執行完，才執行中斷函數。
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// NVIC 初始化
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
}

// 中斷函數的名稱是固定的，見啟動文件 startup_stm32f10x_md.s。
// 不接受任何參數與返回值。
void EXTI15_10_IRQHandler()
{

	// 先判斷是哪個腳位觸發此中斷函數 (10~15都是呼叫此中斷函數。若用EXTI0~4，則無需此步驟。)
	if (EXTI_GetITStatus(EXTI_Line11) == SET)
	{
		// 不可使用這種最簡單的軟體模擬延遲防抖，會導致邏輯錯誤。
		// 要用的話，要搭配 if GPIO_ReadInputDataBit == Set / Reset。
		// 但不如電容穩定。
		// for (volatile int i = 0; i < 100000; i++)
		// {
		// }
		extern volatile uint32_t count;
		count = 0;
	}
	EXTI_ClearITPendingBit(EXTI_Line11); // 清除標誌位須放在最後
}
