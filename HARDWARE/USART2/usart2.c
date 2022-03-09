
#include "stm32f10x.h"	
#include <stm32f10x_conf.h>
#include "usart2.h"	


#define BUFFER_SIZE		16                  //����buf��С                        
unsigned char rxbuf[16],rxempty, rxcnt,rxflag,waitflag;
unsigned short int length_val;
unsigned int timeout;

void USART2_IRQHandler(void)                	//����2�жϷ������
{
	//unsigned char Res;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		if(rxflag==0)
		{
			rxbuf[rxcnt++] = USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
			if(rxcnt>2)
			{
				if(waitflag==1)
				{
					if((rxbuf[rxcnt-2]=='o')&&(rxbuf[rxcnt-1]=='k'))//���ڽ��վ������ݸ�ʽASCII��0mm~2000mm
					{
						waitflag=2;
					}
				}
				else
				{
					if((rxbuf[rxcnt-1]=='m')&&(rxbuf[rxcnt-2]=='m'))//���ڽ��վ������ݸ�ʽASCII��0mm~2000mm
					{
						rxflag=1;
					}
				}
			}
			else if(rxcnt>=16)
			{
				rxcnt=0;
			}
		}
		else
		{
			rxempty = USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
		} 
	} 
} 

void Send_Data_To_UART2(char dat)
{      
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
    USART2->DR = dat;  
}  

void Usart2_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��GPIOAʱ��
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	USART_DeInit(USART2);  //��λ����1
	//USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2

	//USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3

	//Usart2 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
}


