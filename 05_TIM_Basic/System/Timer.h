#ifndef __TIMER_H
#define __TIMER_H

void Timer_Init(void);
void Timer_RCC_Init(void);
void Timer_ETR_and_Infrared_Slot_Sensor_Init(void);
void TIM2_Update_Callback(void);
void TIM1_Update_Callback(void);
#endif
