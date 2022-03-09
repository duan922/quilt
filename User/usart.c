/**
  ******************************************************************************
  
  * @brief   �������ú���
				
  ******************************************************************************
  */
#define USART_C_
#include <stdio.h>
#include "usart.h"
#include "mqtt.h"
#include "BufferManage.h"


/****************************����1****************************/
unsigned char Usart1ReadBuff[Usart1ReadBuffLen]={0};  //�������ݻ���
u32  Usart1ReadCnt = 0;//���ڽ��յ������ݸ���

u8   Usart1ReadBufferMemory[Usart1ReadBufferMemoryLen];//�������ݻ�����
u32  Usart1ReadBufferMemoryManage[Usart1ReadBufferMemoryManageLen];//��������
u8   Usart1ReadBufferMemoryCopy[Usart1ReadBufferMemoryCopyLen];//��ȡ������������

unsigned char Usart1LoopListSendBuff[Usart1LoopListSendBuffLen];  //���ڷ������ݻ���
u8 Usart1SendBits=0;//������ȡ���ζ���1���ֽ�

unsigned char *Usart1SendData;//�����жϷ������ݵĵ�ַ
u32   Usart1SendDataCnt=0;//�����жϷ������ݵĸ���


/****************************����3****************************/
unsigned char Usart3ReadBuff[Usart3ReadBuffLen]={0};  //�������ݻ���
u32  Usart3ReadCnt = 0;//���ڽ��յ������ݸ���

u8   Usart3BufferMemory[Usart3BufferMemoryLen];//�������ݻ�����
u8   Usart3BufferMemoryCopy[Usart3BufferMemoryCopyLen];//��ȡ������������
u32  Usart3ufferMemoryManage[Usart3BufferMemoryManageLen];//��������

unsigned char *Usart3SendData;//�����жϷ������ݵĵ�ַ
u32   Usart3SendDataCnt=0;//�����жϷ������ݵĸ���



/**
* @brief   ���ڳ�ʼ��
* @param   bound  ������
* @param   None
* @param   None
* @retval  None
* @warning None
* @example 
**/
void uart_init(u32 bound1,u32 bound2){
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	rbCreate(&rb_t_usart1_send,Usart1LoopListSendBuff,Usart1LoopListSendBuffLen);//�������ζ���(���ڷ�������)
	/*����1���ջ���*/
	BufferManageCreate(&buff_manage_struct_usart1_read,Usart1ReadBufferMemory,Usart1ReadBufferMemoryLen,Usart1ReadBufferMemoryManage,Usart1ReadBufferMemoryManageLen*4);/*���ڽ��ջ���*/
	/*����3���ջ���*/
	BufferManageCreate(&buff_manage_struct_usart3_read,Usart3BufferMemory,Usart3BufferMemoryLen,Usart3BufferMemoryManage,Usart3BufferMemoryManageLen*4);/*���ڽ��ջ���*/
	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;/*����--1*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;/*����--2*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1 , ENABLE);	//??USART1,GPIOA??
    GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);
	
 //USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
  
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10 
	
  //USART3_TX   GPIOD.8
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //Pc10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  //USART3_RX	  GPIOD.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//Pc11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
	
  //USART ��ʼ������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	
	
	USART_InitStructure.USART_BaudRate = bound1;//���ڲ�����
    USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_InitStructure.USART_BaudRate = bound2;//���ڲ�����
	USART_Init(USART3, &USART_InitStructure); //��ʼ������4
	
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�����ж�
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//�����ж�
	
	
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���
}


/**
* @brief  �����жϷ�������
* @param  c:���ݵ��׵�ַ  cnt:���͵����ݸ���
* @param  None
* @param  None
* @retval None
* @example 
**/
void UsartOutStrIT(USART_TypeDef* USARTx, unsigned char *c, uint32_t cnt)
{
	if(USARTx == USART1){
		PutData(&rb_t_usart1_send,c,cnt);
		USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
	}
	else if(USARTx == USART3){
		Usart3SendData=c;
		Usart3SendDataCnt=cnt;
		USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
		
		PutData(&rb_t_usart1_send,"\r\n>>����:\r\n",11);
		PutData(&rb_t_usart1_send,c,cnt);
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
}


/**
* @brief  ���ڷ����ַ�������
* @param  *c:���͵�����ָ��  cnt:���ݸ���
* @param  None
* @param  None
* @retval None
* @example 
**/
void UsartOutStr(USART_TypeDef* USARTx, unsigned char *c,uint32_t cnt)
{
	while(cnt--)
	{
		USART_SendData(USARTx, *c++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
	}
}


/**
* @brief  ���ڷ���һ���ֽ�
* @param  *c:���͵�����ָ��  cnt:���ݸ���
* @param  None
* @param  None
* @retval None
* @example 
**/
void UsartOutChar(USART_TypeDef* USARTx, unsigned char c)
{
	USART_SendData(USARTx,c);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
}


//����1�жϷ������
void USART1_IRQHandler(void)
{
	u8 Res;
	int value;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		Usart1ReadBuff[Usart1ReadCnt] = Res;	//���յ����ݴ�������
		Usart1ReadCnt++;
		if(Usart1ReadCnt > Usart1ReadBuffLen -10)//��ֹ�������
		{
			Usart1ReadCnt = 0;
		}
	} 
	else if(USART_GetITStatus(USART1,USART_IT_IDLE) == SET)//�����ж�
	{
		USART1->DR; //���USART_IT_IDLE��־
		//�����ݴ��뻺��
	  BufferManageWrite(&buff_manage_struct_usart1_read,Usart1ReadBuff,Usart1ReadCnt,&value);
		Usart1ReadCnt=0;
  }  
	
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
    if(rbCanRead(&rb_t_usart1_send)>0)//�����������ݸ�������0
    {
			rbRead(&rb_t_usart1_send,&Usart1SendBits,1);
			USART_SendData(USART1, Usart1SendBits);
    }
    else
    {
      //�����ֽڽ���			
      USART_ClearITPendingBit(USART1,USART_IT_TXE);
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    }
  }
	//�������
  if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  {
    USART_ClearITPendingBit(USART1,USART_IT_TC);
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);
  }
} 


//����3�жϷ������
void USART3_IRQHandler(void)
{
	u8 Res;
	int value;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
		
		Usart3ReadBuff[Usart3ReadCnt] = Res;	//���յ����ݴ�������
		Usart3ReadCnt++;
		if(Usart3ReadCnt > Usart3ReadBuffLen -10)//��ֹ�������
		{
			Usart3ReadCnt = 0;
		}
	} 
	else if(USART_GetITStatus(USART3,USART_IT_IDLE) == SET)//�����ж�
	{
		USART3->DR; //���USART_IT_IDLE��־
		//�����ݴ��뻺��
	  BufferManageWrite(&buff_manage_struct_usart3_read,Usart3ReadBuff,Usart3ReadCnt,&value);
		
		
		PutData(&rb_t_usart1_send,"\r\n<<����:\r\n",11);
		PutData(&rb_t_usart1_send,Usart3ReadBuff,Usart3ReadCnt);
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		
		
		Usart3ReadCnt=0;
  }  
	
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
  {
    if(Usart3SendDataCnt>0)//�����������ݸ�������0
    {
			USART_SendData(USART3, *Usart3SendData);
			Usart3SendData++;
		  Usart3SendDataCnt--;
    }
    else
    {
      //�����ֽڽ���			
			mymqtt.timer_out_send = 0;//�������ݽ���,���Է�����һ������
			
      USART_ClearITPendingBit(USART3,USART_IT_TXE);
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
      USART_ITConfig(USART3, USART_IT_TC, ENABLE);
    }
  }
	//�������
  if (USART_GetITStatus(USART3, USART_IT_TC) != RESET)
  {
    USART_ClearITPendingBit(USART3,USART_IT_TC);
    USART_ITConfig(USART3, USART_IT_TC, DISABLE);
  }
} 





/**
* @brief  ʹ��microLib�ķ���,ʹ��printf
* @warningg ��ѡ microLib
* @param  None
* @param  None
* @param  None
* @retval None
* @example 
**/

int fputc(int ch, FILE *f)
{
	static char flage=0;
	uint8_t data = ch;
	if(!flage)
	{
	  flage=1;
		USART_ClearITPendingBit(USART1,USART_IT_TC);//���printf��ʧ��һ���ֽڵ�����
	}
	PutData(&rb_t_usart1_send,&data,1);
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  return ch;
}

