#ifndef __TIMER_H
#define __TIMER_H

#ifndef _TIME_C_//如果没有定义  _TIME_C_
#define _TIME_Ex_ extern
#else
#define _TIME_Ex_
#endif

#include "stm32f10x.h"

extern u8 count_led;
extern u8 count_heat;
extern u8 xuan;
void TIM3_Int_Init(u16 arr,u16 psc);

_TIME_Ex_ void Timer2_Config(void);

#endif

