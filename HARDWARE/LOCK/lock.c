
#include "lock.h"
#include "gpio.h" 
 
void LOCK_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LOCK
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC
 GPIO_ResetBits(GPIOC,GPIO_Pin_6);						 //PC.6 �����
    
    	
 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;				 //LOCK
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		     //���ó���������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC
}
 
void light_color(int color)
{
	switch(color)
	{
		case 0:
		{
			LED_red=0;
			LED_yellow=0;
			LED_green=0;
		}break;
		case 1:
		{
			LED_red=1;
			LED_yellow=0;
			LED_green=0;
		}break;
		case 2:
		{
			LED_red=0;
			LED_yellow=0;
			LED_green=1;
		}break;
		case 3:
		{
			LED_red=0;
			LED_yellow=1;
			LED_green=0;
		}break;
			
		
	}
}



