#include "System.h"
#include "uart5.h"

void uart5_init(u32 bound)
{
      //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//使能UART5，GPIOA时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);
 	  USART_DeInit(UART5);  //复位串口5
	 //USART5_TX   PC.12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC.12
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC.12
   
    //USART5_RX	  PD.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化 PD.2

   //Usart5 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	  NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	 USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	 USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(UART5, &USART_InitStructure); //初始化串口
    
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(UART5, ENABLE);                    //使能串口 
}

void UART5_Send_Byte(u8 Data) //发送一个字节；
{
    USART_SendData(UART5,Data);
    while( USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
}

 

void UART5_Send_String(u8 *Data) //发送字符串；
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

void UART5_IRQHandler(void) //中断处理函数；
{
//    u8 res;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断
    {
       USART_ClearFlag(UART5, USART_IT_RXNE); //清除标志位；
//       res=USART_ReceiveData(UART5); //接收数据；
//       UART5_Send_Byte(res); //用户自定义；
//      UART5_Send_Byte(0x42);
    }  
   
} 


