#ifndef __GPIO_H_
#define __GPIO_H_
#include "stm32f10x.h"

#define fan PEout(2)// PE2	
#define heater PEout(3)// PE3	
#define UV_lamp PEout(4)// PE4×ÏÍâµÆk3
#define AC PEout(5)// PE5

#define LED PEout(6)// PE6	
#define LED_red PDout(11)// PD11	
#define LED_yellow PDout(12)// PD12	
#define LED_green PDout(13)// PD13
#define LED_alarm PDout(14)// PD14
#define LED_light PDout(15)// PD15

void GpioInit(void);

#endif
