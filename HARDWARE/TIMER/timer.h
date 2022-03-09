#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
extern u16 count_led;
extern u16 count_heat;
extern u8 xuan;
void TIM3_Int_Init(u16 arr,u16 psc);


#endif
