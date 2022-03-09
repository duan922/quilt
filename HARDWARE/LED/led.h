#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define fan PEout(2)// PE2	
#define heater PEout(3)// PE3	
#define UV_lamp PEout(4)// PE4紫外灯k3
#define AC PEout(5)// PE5

#define LED PEout(6)// PE6	
#define LED_red PDout(11)// PD11	
#define LED_yellow PDout(12)// PD12	
#define LED_green PDout(13)// PD13
#define LED_alarm PDout(14)// PD14
#define LED_light PDout(15)// PD15


void LED_Init(void);//初始化

		 				    
#endif
