#include "led.h"

void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOD, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 // 风扇-->PE.2端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE4
 GPIO_SetBits(GPIOE,GPIO_Pin_2);		

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 // 加热 -->PE.3 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE4
 GPIO_SetBits(GPIOE,GPIO_Pin_3);		    
    
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // uv -->PE.4 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE4
 GPIO_SetBits(GPIOE,GPIO_Pin_4);						 //PE.4 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //AC-->PE.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE5
 GPIO_SetBits(GPIOE,GPIO_Pin_5);	
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED-->PE.6 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE6
 GPIO_SetBits(GPIOE,GPIO_Pin_6);
 
/*   指示灯    */
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //LED-->PD端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE6
 GPIO_ResetBits(GPIOD,GPIO_Pin_6);
 
 
 
 
}
 
