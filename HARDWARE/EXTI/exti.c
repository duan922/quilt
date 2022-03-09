#include "exti.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x_exti.h"
#include "wk2xxx.h"

EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
//外部中断0服务程序
void EXTIX_Init(void)
{
 
 	//EXTI_InitTypeDef EXTI_InitStructure;
 	//NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

  //GPIOA.1 中断线以及中断初始化配置   下降沿触发
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource1);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键KEY2所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);
 
}

//
void Exti_Enable(void)
{
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键KEY2所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);
}

void Exti_Disable(void)
{
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键KEY2所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);
}
float AC_Voltage,AC_Current;
void EXTI1_IRQHandler(void)
{ 
	 u8 gifr,pzem_rxbuf[25];
	 int rxlen;
    

	if(EXTI_GetFlagStatus(EXTI_Line1)!= RESET)
	{   	EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE1上的中断标志位  
			gifr=WkReadGReg(WK2XXX_GIFR);/**/
		do{
			if(gifr&WK2XXX_UT1INT)//判断子串口1是否有中断
			{ /*数据处理*/
						/*数据接收*/
						rxlen=wk_RxChars(1,pzem_rxbuf);//一次接收的数据不会超过256Byte
						/*数据发送*/
						//把接收的数据发送出去
//						wk_TxChars(1,rxlen,rxbuf);
//                       printf("已接收\r\n");
                  AC_Voltage=(pzem_rxbuf[3]*256+pzem_rxbuf[4])/10.0;//计算电压值 
                  AC_Current=(pzem_rxbuf[5]*256+pzem_rxbuf[6])/1000.0;//计算电流值
                

			}
			
			if(gifr&WK2XXX_UT2INT)//判断子串口2是否有中断
			{
				/*数据接收*/
						rxlen=wk_RxChars(2,pzem_rxbuf);//一次接收的数据不会超过256Byte
						/*数据发送*/
						//把接收的数据发送出去
						wk_TxChars(2,rxlen,pzem_rxbuf);
				        
			
			  }
			if(gifr&WK2XXX_UT3INT)//判断子串口3是否有中断
			{
				/*数据接收*/
						rxlen=wk_RxChars(3,pzem_rxbuf);//一次接收的数据不会超过256Byte
						/*数据发送*/
						//把接收的数据发送出去
//						wk_TxChars(3,rxlen,rxbuf);
					
			}
			if(gifr&WK2XXX_UT4INT)//判断子串口4是否有中断
			{
				/*数据接收*/
						rxlen=wk_RxChars(4,pzem_rxbuf);//一次接收的数据不会超过256Byte
						/*数据发送*/
						//把接收的数据发送出去
				   //wk_TxChars(4,rxlen,rxbuf);
						
			}
			
		gifr=WkReadGReg(WK2XXX_GIFR);
		//printf("IN EXTI2_IRQ GIFR:0X%X !!!\n",gifr);
		}while(gifr&0x0f);
//		printf("IN EXTI1_IRQ over! !!!\n");
	}
}
