#include "gpio.h"

/**
* @brief  设置用到的GPIO

**/
void GpioInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);	 //??PA????
			    
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;	//继电器|复位模块引脚			 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

      
       // 风扇-->PE.2 .加热器 -->PE.3 .紫外uv -->PE.4 .电流AC-->PE.5 .灯LED-->PE.6
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;				 
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
     GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE4
     GPIO_SetBits(GPIOE,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);						 //默认PE.4 输出高
    
     
    /*   指示灯    */
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;			//复位模块引脚PD10	 //LED-->PD端口配置
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
     GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE6
     GPIO_ResetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
 
}
