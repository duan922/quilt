#ifndef __EXTI_H
#define __EXTI_H	 
#include "System.h"
#include "stm32f10x_exti.h"
//EXTI_InitTypeDef EXTI_InitStructure;
//NVIC_InitTypeDef NVIC_InitStructure;
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ���ο�����ʹ�ã�δ��������ɣ��������������κ���;
//WK2124 DEMO
//STM32 �ⲿ�ж�  
//�ɶ�Ϊ��΢����
//������̳:www.wkmic.com
//�޸�����:2017/8/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//////////////////////////////////////////////////////////////////////////////////  

extern float AC_Voltage,AC_Current;
void EXTIX_Init(void);//�ⲿ�жϳ�ʼ��		 
void Exti_Enable(void);
void Exti_Disable(void);
#endif

