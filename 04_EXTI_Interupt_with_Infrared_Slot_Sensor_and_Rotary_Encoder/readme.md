# 專案功能
1. 使用紅外線遮斷感測器 (Infrared Slot Sensor)與旋轉編碼器 (Rotary_Encoder)觸發中斷函數。控制 OLED 內容。
2. OLED內容: 順時針轉++, 逆時針轉--, 遮斷紅外線清0
3. 學習使用 EXTI, AFIO, NVIC。

# 額外練習
使用 extern, volatile 來控制中斷函數的計數器。

# 外設接線
- OLED SCL: GPIOC_Pin_14
- OLED SDA: GPIOC_Pin_15
- 紅外線遮斷感測器 (Infrared Slot Sensor): GPIOB_Pin_11，需額外並聯 104 陶瓷電容接地來濾波。
- 旋轉編碼器 (Rotary_Encoder) Pin A (旋轉): GPIOB_Pin_8
- 旋轉編碼器 (Rotary_Encoder) Pin B (旋轉): GPIOB_Pin_9

# 錯誤備忘錄
- 感測訊號抖動: 需手動把訊號使用濾波陶瓷電容並聯接地，才能有正確的下降觸發。不可使用最簡的軟體模擬延遲， `for (volatile int i = 0; i < 100000; i++)` 會變成上升觸發。若同時使用電容，則完全不觸發。
- 電流驅動力不足: 在上一個專案當中，誤把 OLED 的 SPI 訊號線接到 GPIOC 13 上 (此引腳同時控制開發版上的藍色 LED)。雖然在只點燈一次的環境可以正確運作，但是後續刷新時，由於電流驅動力不足，導致 OLED 無法更新。並且會導致板載 LED 狂閃。