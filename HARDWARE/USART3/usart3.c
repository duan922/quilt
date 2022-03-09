
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"	
#include <stm32f10x_conf.h>
#include "usart3.h"	


#define BUFFER_SIZE		16
/* Private variables ---------------------------------------------------------*/
unsigned char rxbuf[16],rxempty, rxcnt,rxflag,waitflag;
unsigned short int length_val;
unsigned int timeout;
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
 
/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : usart interrupt function
* Input          : bound-baud
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)                	//串口1中断服务程序
{
	//unsigned char Res;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		if(rxflag==0)
		{
			rxbuf[rxcnt++] = USART_ReceiveData(USART3);//(USART1->DR);	//读取接收到的数据
			if(rxcnt>2)
			{
				if(waitflag==1)
				{
					if((rxbuf[rxcnt-2]=='o')&&(rxbuf[rxcnt-1]=='k'))//串口接收距离数据格式ASCII码0mm~2000mm
					{
						waitflag=2;
					}
				}
				else
				{
					if((rxbuf[rxcnt-1]=='m')&&(rxbuf[rxcnt-2]=='m'))//串口接收距离数据格式ASCII码0mm~2000mm
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
			rxempty = USART_ReceiveData(USART3);//(USART2->DR);	//读取接收到的数据
		} 
	} 
} 
/*******************************************************************************
* Function Name  : Send_Data_To_UART1
* Description    : send a char to uart
* Input          : dat
* Output         : None
* Return         : None
*******************************************************************************/
void Send_Data_To_UART3(char dat)
{      
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
    USART3->DR = dat;  
}  
/*******************************************************************************
* Function Name  : uart_init
* Description    : config usart1
* Input          : bound-baud
* Output         : None
* Return         : None
*******************************************************************************/
void Usart2_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	USART_DeInit(USART3);  //复位串口1
	//USART1_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2

	//USART1_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3

	//Usart1 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
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

	USART_Init(USART3, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART3, ENABLE);                    //使能串口 
}


/************************END OF FILE*************************/
