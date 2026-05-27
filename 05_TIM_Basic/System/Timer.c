#include "stm32f10x.h"
#include "Timer.h"

void Timer_Init()
{
	Timer_RCC_Init();
	Timer_ETR_and_Infrared_Slot_Sensor_Init();
}

void Timer_RCC_Init()
{
	// 開啟 TIM2 的核心時脈
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	// 設定 TIM2 的計數時脈來源 (RCC內部(內核)時脈 / ETR外部(周邊)時脈 / ITRx其他TIM觸發 / TIx捕獲通道)
	// RM page 254 figure98: 通用定時器框圖
	TIM_InternalClockConfig(TIM2);
	// TIM_ETRClockMode1Config(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter);
	// TIM_ETRClockMode2Config(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter);
	// TIM_ITRxExternalClockConfig(TIMx, TIM_InputTriggerSource);
	// TIM_TIxExternalClockConfig(TIMx, TIM_TIxExternalCLKSource, TIM_ICPolarity, ICFilter);

	// 設定 TIMx 模式
	// 經過兩/三(僅高級計時器)層除頻以後 才輸出一次中斷訊號
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	// 濾波採樣頻率: 僅對 ETR, TIx 這兩種外部輸入時脈的模式有用。頻率低可防抖，但延遲高。
	// 原理: 以RCC分頻後的頻率(72M/x)，連續採樣N次。
	// 若"連續"採樣到N次相同，TIM的電位才會變化(進行半次記數)。
	// N 在 ERT (TIM_ETRConfig(,,, ExtTRGFilter)), TI (TIM_ICInit(TIM_ICInitStruct)) 內設定。
	// 也就是說外部頻率需要限制 <= 72M / DIVx / N /2
	// 其中 72M / DIVx 是實際採樣頻率，N是需要滿足的連續檢查次數，/2是為了滿足奈奎斯特採樣定理(採樣頻率需要為實際頻率的兩倍)。
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	 // 濾波採樣頻率
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 計數器模式可以設置 遞增(向上), 遞減(向下), 輪流遞增遞減(中央計數)
	// 除頻至 1 Hz 觸發中斷
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;		 // 第二層除頻, 數值需 +1 , max 0xffff (65535)
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;	 // 第一層除頻, 數值需 +1 , max 0xffff (65535)
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00; // 第三層除頻 (僅高級計時器 TIM1 & TIM8 才有用，這裡可以隨便設)
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

	// TIM_TimeBaseInit 為了更新 Period & Prescaler，會立即更新觸發一次事件，否則無法更新影子暫存器。
	// 但是更新事件，會導致初始上電就觸發一次ISR。因此需要在NVIC開啟以前，就手動把中斷標誌位清除。避免上電即觸發ISR。
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	// 使能 TIM 的 IT (中斷)
	// TIM_IT_Update 更新中斷;					// 數到 CNT == ARR 觸發中斷，執行ISR。 // 數到ARR後，CNT會歸零，而CCR不會歸零。CCR相當於在ARR裡面拆出更多次中斷，最多設置4個CCR，所以一個ARR週期裡面可以有5次中斷。雖然會進同一個ISR (TIM2_IRQHandler)，但可以if判斷五種不同的功能。
	// TIM_IT_CCx 捕獲(輸入) 比較(輸出) 中斷;	 // 捕獲:讀取計數器的值，比較:數到CNT == CCR 觸發中斷，執行ISR。
	// TIM_IT_Trigger 觸發中斷;				    // TIM 由其他 TIM 觸發"事件"時，捕獲"事件"的觸發訊號，並觸發中斷訊號，執行ISR。(不設置的話，訊號會繞過CPU運作，不觸發中斷。) (事件:硬體發出脈衝訊號，不一定是給CPU響應, 中斷:向CPU發出執行ISR的訊號)
	// TIM_IT_COM 換向中斷(僅高級定時器可用);	 // 馬達切換下一相通電狀態時，由馬達上的感測器自己觸發中斷。馬達硬體會自動高速切換至下一相的PWM輸出通道。隨後中斷傳達給CPU執行ISR，預載入下下一相的配置至Preload reg當中。
	// TIM_IT_Break 煞車中斷(僅高級定時器可用);  // 可設置按鈕或防撞感測器，硬體直接先自動高速關閉所有PWM通道，接著才觸發此中斷，執行ISR，記錄錯誤訊息或復位處理。
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	// 使能計數器
	TIM_Cmd(TIM2, ENABLE);

	// 設置中斷優先級分組 (整個硬體共通，跨檔案/感測器須確保設置一致!!!)
	// 總共 4 bits, 要分配給搶占優先級&響應優先級分別個幾 bits。
	// 搶占優先級: 中斷當前函式，優先執行中斷函數。
	// 響應優先級: 當前函式執行完，才執行中斷函數。
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// NVIC 初始化
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
}

// 在 f103c8t6 上面僅有 TIM1 & TIM2 有預設的 ETR 腳位可用 (僅有 PA ~ PC 引腳)。
// TIM2_CH1_ETR: PA0
// TIM1_ETR : PA12
// TIM3_ETR : PD2
// TIM4_ETR : PE0
void Timer_ETR_and_Infrared_Slot_Sensor_Init()
{
	// 開啟 TIM1 的核心時脈
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // GPIOA12 = TIM1_ETR

	// 初始化 TIM1_ETR
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// 計數時脈來源
	// 把 ETR 輸入，當作此 TIM 的時脈
	TIM_ETRClockMode2Config(TIM1, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x0F);

	// 設定 TIMx 模式
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);

	// 使能計數器
	TIM_Cmd(TIM1, ENABLE);

	// 使能 TIM 的 IT (中斷)
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

	// 設置中斷優先級分組
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// NVIC 初始化
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
}

void TIM2_Update_Callback(void)
{
	extern volatile uint32_t count;
	count++;
}

void TIM1_Update_Callback(void)
{
	extern volatile uint32_t count;
	count = 0;
}
