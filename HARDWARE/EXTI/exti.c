#include "exti.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x_exti.h"
#include "wk2xxx.h"

EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
//�ⲿ�ж�0�������
void EXTIX_Init(void)
{
 
 	//EXTI_InitTypeDef EXTI_InitStructure;
 	//NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

  //GPIOA.1 �ж����Լ��жϳ�ʼ������   �½��ش���
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource1);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ʹ�ܰ���KEY2���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);
 
}

//
void Exti_Enable(void)
{
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ʹ�ܰ���KEY2���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);
}

void Exti_Disable(void)
{
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ʹ�ܰ���KEY2���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);
}
float AC_Voltage,AC_Current;
void EXTI1_IRQHandler(void)
{ 
	 u8 gifr,pzem_rxbuf[25];
	 int rxlen;
    

	if(EXTI_GetFlagStatus(EXTI_Line1)!= RESET)
	{   	EXTI_ClearITPendingBit(EXTI_Line1); //���LINE1�ϵ��жϱ�־λ  
			gifr=WkReadGReg(WK2XXX_GIFR);/**/
		do{
			if(gifr&WK2XXX_UT1INT)//�ж��Ӵ���1�Ƿ����ж�
			{ /*���ݴ���*/
						/*���ݽ���*/
						rxlen=wk_RxChars(1,pzem_rxbuf);//һ�ν��յ����ݲ��ᳬ��256Byte
						/*���ݷ���*/
						//�ѽ��յ����ݷ��ͳ�ȥ
//						wk_TxChars(1,rxlen,rxbuf);
//                       printf("�ѽ���\r\n");
                  AC_Voltage=(pzem_rxbuf[3]*256+pzem_rxbuf[4])/10.0;//�����ѹֵ 
                  AC_Current=(pzem_rxbuf[5]*256+pzem_rxbuf[6])/1000.0;//�������ֵ
                

			}
			
			if(gifr&WK2XXX_UT2INT)//�ж��Ӵ���2�Ƿ����ж�
			{
				/*���ݽ���*/
						rxlen=wk_RxChars(2,pzem_rxbuf);//һ�ν��յ����ݲ��ᳬ��256Byte
						/*���ݷ���*/
						//�ѽ��յ����ݷ��ͳ�ȥ
						wk_TxChars(2,rxlen,pzem_rxbuf);
				        
			
			  }
			if(gifr&WK2XXX_UT3INT)//�ж��Ӵ���3�Ƿ����ж�
			{
				/*���ݽ���*/
						rxlen=wk_RxChars(3,pzem_rxbuf);//һ�ν��յ����ݲ��ᳬ��256Byte
						/*���ݷ���*/
						//�ѽ��յ����ݷ��ͳ�ȥ
//						wk_TxChars(3,rxlen,rxbuf);
					
			}
			if(gifr&WK2XXX_UT4INT)//�ж��Ӵ���4�Ƿ����ж�
			{
				/*���ݽ���*/
						rxlen=wk_RxChars(4,pzem_rxbuf);//һ�ν��յ����ݲ��ᳬ��256Byte
						/*���ݷ���*/
						//�ѽ��յ����ݷ��ͳ�ȥ
				   //wk_TxChars(4,rxlen,rxbuf);
						
			}
			
		gifr=WkReadGReg(WK2XXX_GIFR);
		//printf("IN EXTI2_IRQ GIFR:0X%X !!!\n",gifr);
		}while(gifr&0x0f);
//		printf("IN EXTI1_IRQ over! !!!\n");
	}
}
