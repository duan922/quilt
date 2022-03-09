/**
  ******************************************************************************
  
  * @brief   串口配置函数
				
  ******************************************************************************
  */
#define USART_C_
#include <stdio.h>
#include "usart.h"
#include "mqtt.h"
#include "BufferManage.h"


/****************************串口1****************************/
unsigned char Usart1ReadBuff[Usart1ReadBuffLen]={0};  //接收数据缓存
u32  Usart1ReadCnt = 0;//串口接收到的数据个数

u8   Usart1ReadBufferMemory[Usart1ReadBufferMemoryLen];//接收数据缓存区
u32  Usart1ReadBufferMemoryManage[Usart1ReadBufferMemoryManageLen];//管理缓存区
u8   Usart1ReadBufferMemoryCopy[Usart1ReadBufferMemoryCopyLen];//提取缓存区的数据

unsigned char Usart1LoopListSendBuff[Usart1LoopListSendBuffLen];  //串口发送数据缓存
u8 Usart1SendBits=0;//串口提取环形队列1个字节

unsigned char *Usart1SendData;//串口中断发送数据的地址
u32   Usart1SendDataCnt=0;//串口中断发送数据的个数


/****************************串口3****************************/
unsigned char Usart3ReadBuff[Usart3ReadBuffLen]={0};  //接收数据缓存
u32  Usart3ReadCnt = 0;//串口接收到的数据个数

u8   Usart3BufferMemory[Usart3BufferMemoryLen];//接收数据缓存区
u8   Usart3BufferMemoryCopy[Usart3BufferMemoryCopyLen];//提取缓存区的数据
u32  Usart3ufferMemoryManage[Usart3BufferMemoryManageLen];//管理缓存区

unsigned char *Usart3SendData;//串口中断发送数据的地址
u32   Usart3SendDataCnt=0;//串口中断发送数据的个数



/**
* @brief   串口初始化
* @param   bound  波特率
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
	
	
	rbCreate(&rb_t_usart1_send,Usart1LoopListSendBuff,Usart1LoopListSendBuffLen);//创建环形队列(用于发送数据)
	/*串口1接收缓存*/
	BufferManageCreate(&buff_manage_struct_usart1_read,Usart1ReadBufferMemory,Usart1ReadBufferMemoryLen,Usart1ReadBufferMemoryManage,Usart1ReadBufferMemoryManageLen*4);/*串口接收缓存*/
	/*串口3接收缓存*/
	BufferManageCreate(&buff_manage_struct_usart3_read,Usart3BufferMemory,Usart3BufferMemoryLen,Usart3BufferMemoryManage,Usart3BufferMemoryManageLen*4);/*串口接收缓存*/
	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;/*串口--1*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;/*串口--2*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1 , ENABLE);	//??USART1,GPIOA??
    GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);
	
 //USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
  
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10 
	
  //USART3_TX   GPIOD.8
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //Pc10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  //USART3_RX	  GPIOD.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//Pc11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
	
  //USART 初始化设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	
	
	USART_InitStructure.USART_BaudRate = bound1;//串口波特率
    USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_InitStructure.USART_BaudRate = bound2;//串口波特率
	USART_Init(USART3, &USART_InitStructure); //初始化串口4
	
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//空闲中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//空闲中断
	
	
  USART_Cmd(USART1, ENABLE);                    //使能串口
	USART_Cmd(USART3, ENABLE);                    //使能串口
}


/**
* @brief  串口中断发送数据
* @param  c:数据的首地址  cnt:发送的数据个数
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
		
		PutData(&rb_t_usart1_send,"\r\n>>发送:\r\n",11);
		PutData(&rb_t_usart1_send,c,cnt);
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
}


/**
* @brief  串口发送字符串数据
* @param  *c:发送的数据指针  cnt:数据个数
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
* @brief  串口发送一个字节
* @param  *c:发送的数据指针  cnt:数据个数
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


//串口1中断服务程序
void USART1_IRQHandler(void)
{
	u8 Res;
	int value;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		Usart1ReadBuff[Usart1ReadCnt] = Res;	//接收的数据存入数组
		Usart1ReadCnt++;
		if(Usart1ReadCnt > Usart1ReadBuffLen -10)//防止数组溢出
		{
			Usart1ReadCnt = 0;
		}
	} 
	else if(USART_GetITStatus(USART1,USART_IT_IDLE) == SET)//空闲中断
	{
		USART1->DR; //清除USART_IT_IDLE标志
		//把数据存入缓存
	  BufferManageWrite(&buff_manage_struct_usart1_read,Usart1ReadBuff,Usart1ReadCnt,&value);
		Usart1ReadCnt=0;
  }  
	
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
    if(rbCanRead(&rb_t_usart1_send)>0)//如果里面的数据个数大于0
    {
			rbRead(&rb_t_usart1_send,&Usart1SendBits,1);
			USART_SendData(USART1, Usart1SendBits);
    }
    else
    {
      //发送字节结束			
      USART_ClearITPendingBit(USART1,USART_IT_TXE);
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    }
  }
	//发送完成
  if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  {
    USART_ClearITPendingBit(USART1,USART_IT_TC);
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);
  }
} 


//串口3中断服务程序
void USART3_IRQHandler(void)
{
	u8 Res;
	int value;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
		
		Usart3ReadBuff[Usart3ReadCnt] = Res;	//接收的数据存入数组
		Usart3ReadCnt++;
		if(Usart3ReadCnt > Usart3ReadBuffLen -10)//防止数组溢出
		{
			Usart3ReadCnt = 0;
		}
	} 
	else if(USART_GetITStatus(USART3,USART_IT_IDLE) == SET)//空闲中断
	{
		USART3->DR; //清除USART_IT_IDLE标志
		//把数据存入缓存
	  BufferManageWrite(&buff_manage_struct_usart3_read,Usart3ReadBuff,Usart3ReadCnt,&value);
		
		
		PutData(&rb_t_usart1_send,"\r\n<<接收:\r\n",11);
		PutData(&rb_t_usart1_send,Usart3ReadBuff,Usart3ReadCnt);
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		
		
		Usart3ReadCnt=0;
  }  
	
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
  {
    if(Usart3SendDataCnt>0)//如果里面的数据个数大于0
    {
			USART_SendData(USART3, *Usart3SendData);
			Usart3SendData++;
		  Usart3SendDataCnt--;
    }
    else
    {
      //发送字节结束			
			mymqtt.timer_out_send = 0;//发送数据结束,可以发送下一条数据
			
      USART_ClearITPendingBit(USART3,USART_IT_TXE);
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
      USART_ITConfig(USART3, USART_IT_TC, ENABLE);
    }
  }
	//发送完成
  if (USART_GetITStatus(USART3, USART_IT_TC) != RESET)
  {
    USART_ClearITPendingBit(USART3,USART_IT_TC);
    USART_ITConfig(USART3, USART_IT_TC, DISABLE);
  }
} 





/**
* @brief  使用microLib的方法,使用printf
* @warningg 勾选 microLib
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
		USART_ClearITPendingBit(USART1,USART_IT_TC);//解决printf丢失第一个字节的问题
	}
	PutData(&rb_t_usart1_send,&data,1);
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  return ch;
}

