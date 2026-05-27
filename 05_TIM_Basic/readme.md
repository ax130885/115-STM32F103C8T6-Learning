# 專案功能
1. 學習使用定時器，觸發通用中斷。
2. 系統主頻(72MHz)或是內部外設的時脈(需引腳重定義)。透過預分頻器、計數器，兩層過濾，才輸出一個更新中斷。
3. 定時器 TIM2 每秒呼叫一次中斷 count++，紅外線遮斷透過 TIM1 計數器，數 2 次就歸零。


# 外設接線
- OLED SCL: GPIOC_Pin_14
- OLED SDA: GPIOC_Pin_15
- 紅外線遮斷感測器 DO: GPIOA_Pin_12 (TIM1_ETR)


# 額外學習
將中斷函數整合至 `stm32f10x_it.c` (目錄)，  
透過目錄調用在對應檔案內的具體實作。


# 定時器分組 
stm32f103c8t6 僅有 TIM[1:4] !!  
不要看RM亂寫  
要看 `stm32f10x.h` 裡面  
`L246: ifdef STM32F10X_MD` 真正有被定義的腳位  
- 高級定時器 (APB2): TIM1, TIM8
- 通用定時器 (APB1): TIM2, TIM3, TIM4, TIM5
- 基本定時器 (APB1): TIM6, TIM7


# TIM 的 ETR (外部觸發) 引腳
由 datasheet page 18 ~ 21，直接搜尋 ETR 可知。
在 stm32f103c8t6 上面僅有 TIM1 & TIM2 有預設的 ETR 腳位可用 (僅有 PA ~ PC 引腳)。  
- TIM2_CH1_ETR: PA0
- TIM1_ETR : PA12
- TIM3_ETR : PD2
- TIM4_ETR : PE0
- 