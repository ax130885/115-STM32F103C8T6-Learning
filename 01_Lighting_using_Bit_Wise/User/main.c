#include "stm32f10x.h" // Device header
#ifdef USE_STDPERIPH_DRIVER
#include "stm32f10x_conf.h"
#endif

// 使用暫存器操作 (下面有使用標準外設庫操作的版本)
int main(void)
{
	// 目的: 點燈
	// 想法: 由最小系統圖可知，LED燈的負極接到stm32的PC13上。
	//       由datasheet page 14 fiugre 1可知，PC對應到GPIOC。
	// 結論: 要控制硬體的電燈PC13，對應到軟體的GPIOC13腳位

	// Step1. 打開GPIOC的時脈/電源(RCC)
	// RM page 25: 電路架構可知，RCC透過APB2匯流排接到GPIOC
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // RM Page 70: bit 4, IOPCEN, 0 off, 1 on

	// Step2. 設定(配置)GPIOC13的操作模式
	GPIOC->CRH |= GPIO_CRH_MODE13; // RM Page 114: CNF13=00 (Out, Push-Pull), MODE13=11(Output 50Hz)

	// Step3. 實際對GPIOC13進行IO操作 (I: IDR, O: ODR)
	// 由最小系統圖可知，LED燈的負極接到stm32的PC13上。
	// 因此是GPIOC13為低電位亮燈，高電位熄燈。
	GPIOC->ODR |= GPIO_ODR_ODR13; // RM Page 115: 設置高電位 熄燈
								  // GPIOC->ODR &= ~GPIO_ODR_ODR13;     // 設置低電位 開燈 (復位值)

	// // 閃爍
	// while(1){
	// 	GPIOC->ODR &= ~GPIO_ODR_ODR13; // 亮
	//     for(int i=0; i<1000000; i++);  // 粗糙的軟體延遲

	//     GPIOC->ODR |= GPIO_ODR_ODR13;  // 暗
	//     for(int i=0; i<1000000; i++);
	// }
}

// 使用標準外設庫實作
int main2(void)
{
	// Step1. 打開GPIOC的時脈/電源(RCC)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 變更哪一組的RCC, 開啟/關閉

	// Step2. 設定(配置)GPIOC13的操作模式
	GPIO_InitTypeDef GPIO_InitStruct;			   // 點開定義發現有三個屬性, 從後面的 @ref 去搜關鍵字, 可得該填入的值
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  // GPIOMode_TypeDef
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;		   // GPIO_pins_define
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // GPIOSpeed_TypeDef
	GPIO_Init(GPIOC, &GPIO_InitStruct);			   // 變更哪一組GPIO, 具體設定(要求傳入一個GPIO_InitTypeDef結構體的指標, 建議名稱為GPIO_InitStruct 詳見原始標頭定義)

	// Step3. 實際對GPIOC13進行IO操作
	GPIO_SetBits(GPIOC, GPIO_Pin_13); // 設置高電位 熄燈
									  // GPIO_ResetBits(GPIOC, GPIO_Pin_13); // 設置低電位 開燈 (復位值)

	// // 閃爍
	// while(1){
	// 	GPIO_SetBits(GPIOC, GPIO_Pin_13);    // 暗
	// 	for(int i=0; i<1000000; i++);

	// 	GPIO_ResetBits(GPIOC, GPIO_Pin_13);  // 亮
	// 	for(int i=0; i<1000000; i++);
	// }
}
