#include "System.h"
#include "uart5.h"

void uart5_init(u32 bound)
{
      //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//ʹ��UART5��GPIOAʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);
 	  USART_DeInit(UART5);  //��λ����5
	 //USART5_TX   PC.12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC.12
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PC.12
   
    //USART5_RX	  PD.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOD, &GPIO_InitStructure);  //��ʼ�� PD.2

   //Usart5 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
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
    USART_Init(UART5, &USART_InitStructure); //��ʼ������
    
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ��� 
}

void UART5_Send_Byte(u8 Data) //����һ���ֽڣ�
{
    USART_SendData(UART5,Data);
    while( USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
}

 

void UART5_Send_String(u8 *Data) //�����ַ�����
{
    while(*Data)
    UART5_Send_Byte(*Data++);
} 


unsigned char uart5_recByte(void) 
{ 	
	  uint16_t Data,t; 
	  while(!USART_GetFlagStatus(UART5,USART_FLAG_RXNE))
      {if(t++>1000) break;}
		Data=USART_ReceiveData(UART5);
		return (Data & 0xff); 	
}

void UART5_IRQHandler(void) //�жϴ�������
{
//    u8 res;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�
    {
       USART_ClearFlag(UART5, USART_IT_RXNE); //�����־λ��
//       res=USART_ReceiveData(UART5); //�������ݣ�
//       UART5_Send_Byte(res); //�û��Զ��壻
//      UART5_Send_Byte(0x42);
    }  
   
} 


