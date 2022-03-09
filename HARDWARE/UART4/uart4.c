#include "sys.h"
#include "uart4.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "sim800a.h"
#include "usart.h"
#include "delay.h"
#include "led.h"


//���ڽ��ջ�����
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN]; 	//���ͻ���,���UART4_MAX_SEND_LEN�ֽ�	  1024
//���ڷ��ͻ�����
u8 UART4_TX_BUF[UART4_MAX_SEND_LEN]; 	//���ջ���,���UART4_MAX_RECV_LEN���ֽ�  256


u8 tcp_rx[255];//���շ�����������ָ����ڴ���4�жϺ�����

void uart4_init(u32 bound)
{
  //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//ʹ��USART4��GPIOCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	
  
	//UART4_TX    GPIOC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.10   
	
  //UART4_RX	  GPIOC.11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.11  

  //Uart4 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
  //UART4 ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART4, &USART_InitStructure); //��ʼ������4
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ���4
	
	TIM4_Init(99,7199);		//10ms�ж�
	TIM4_Set(0);			//�رն�ʱ��4 					
	UART4_RX_STA=0;		//����
}

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 UART4_RX_STA=0;
u8 rev_start=0;
u8 number=0;
char  GET_num; 

void UART4_IRQHandler(void)
{
	 u8 res;
	 
   if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET)//���յ�����
	 {	 
		  res=USART_ReceiveData(UART4); 
		  
         //ָ��������֣��������������������ض��ĸ�ʽ������ +IPD,1:1234//+MSUB: "mqtt/topic",9 byte,SSSSddddd
		  if(res=='+')//��ʼλ
			{
				rev_start=1;
				number=0;
			}
			
			if(rev_start==1)
			{
				tcp_rx[number++]=res;
				if(res=='\n')//����λ
				{
					tcp_rx[number]='\0';//���������־
					rev_start=0;
					number=0;
                    printf("%c\r\n",tcp_rx[22]);
                   //  printf("%d\r\n",tcp_rx[26]);
                    // printf("%c\r\n",tcp_rx[27]);
                    
                    if(tcp_rx[21]=='2'||tcp_rx[22]=='2')
                    { //printf("a");
                        GET_num='2'; }
                    else if(tcp_rx[21]=='3')
                        GET_num='3';
                    else if(tcp_rx[21]=='4')
                        GET_num='4';
                    else if(tcp_rx[21]=='5')
                        GET_num='5';
                  
				}
			}
		 
		  if(UART4_RX_STA<UART4_MAX_RECV_LEN)		//�����Խ�������
		  {
		    TIM_SetCounter(TIM4,0);//���������      
			  if(UART4_RX_STA==0)
					TIM4_Set(1);	 	//ʹ�ܶ�ʱ��4���ж�
			  UART4_RX_BUF[UART4_RX_STA++]=res;		//��¼���յ���ֵ***************************              
		  }
			else 
		  {
			  UART4_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������

          }			
			
	 }  											 
}   
//����4,printf ����
//ȷ��һ�η������ݲ�����UART4_MAX_SEND_LEN�ֽ�
void u4_printf(char* fmt,...)  
{ 
	u16 len,i;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)UART4_TX_BUF,fmt,ap); //����
	va_end(ap);
	
	len=strlen((const char*)UART4_TX_BUF);
	i=0;
	USART_ClearFlag(UART4, USART_FLAG_TC);
	USART_ClearITPendingBit(UART4, USART_FLAG_TXE);
	while(len--)
	{
		USART_SendData(UART4, *(UART4_TX_BUF+i));
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) != 1);
		USART_ClearFlag(UART4, USART_FLAG_TC);
		USART_ClearITPendingBit(UART4, USART_FLAG_TXE);
		i++;
	}
}


//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM4_Init(u16 arr,u16 psc)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);      //ʱ��ʹ��//TIM4ʱ��ʹ��  
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);           //����������2
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM_DeInit(TIM4);                                         //��λTIM4��ʱ�� 
	TIM_TimeBaseStructure.TIM_Period=arr;                     //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler=psc;                  //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);                     //�����ʱ��4�������־λ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);           //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE );                 //ʹ��ָ����TIM4�ж�,��������ж�
	//TIM_Cmd(TIM4, ENABLE);      
}

//����TIM4�Ŀ���
//sta:0���ر�;1,����;
void TIM4_Set(u8 sta)
{
	if(sta)
	{  
		TIM_SetCounter(TIM4,0);//���������
		TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx	
	}
	else 
		TIM_Cmd(TIM4, DISABLE);//�رն�ʱ��4
}

//��ʱ��4�жϷ������		    
void TIM4_IRQHandler(void)
{ 	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)//�Ǹ����ж�
	{	 			   
		UART4_RX_STA|=1<<15;	//��ǽ������
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //���TIMx�����жϱ�־    
		TIM4_Set(0);			//�ر�TIM4  
	}   
}


