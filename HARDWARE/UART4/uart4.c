#include "sys.h"
#include "uart4.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "sim800a.h"
#include "usart.h"
#include "delay.h"
#include "led.h"


//串口接收缓存区
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN]; 	//发送缓冲,最大UART4_MAX_SEND_LEN字节	  1024
//串口发送缓存区
u8 UART4_TX_BUF[UART4_MAX_SEND_LEN]; 	//接收缓冲,最大UART4_MAX_RECV_LEN个字节  256


u8 tcp_rx[255];//接收服务器发来的指令，用在串口4中断函数里

void uart4_init(u32 bound)
{
  //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//使能USART4，GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	
  
	//UART4_TX    GPIOC.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.10   
	
  //UART4_RX	  GPIOC.11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.11  

  //Uart4 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
  //UART4 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(UART4, &USART_InitStructure); //初始化串口4
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(UART4, ENABLE);                    //使能串口4
	
	TIM4_Init(99,7199);		//10ms中断
	TIM4_Set(0);			//关闭定时器4 					
	UART4_RX_STA=0;		//清零
}

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 UART4_RX_STA=0;
u8 rev_start=0;
u8 number=0;
char  GET_num; 

void UART4_IRQHandler(void)
{
	 u8 res;
	 
   if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET)//接收到数据
	 {	 
		  res=USART_ReceiveData(UART4); 
		  
         //指令解析部分，服务器发来的数据有特定的格式，例如 +IPD,1:1234//+MSUB: "mqtt/topic",9 byte,SSSSddddd
		  if(res=='+')//起始位
			{
				rev_start=1;
				number=0;
			}
			
			if(rev_start==1)
			{
				tcp_rx[number++]=res;
				if(res=='\n')//结束位
				{
					tcp_rx[number]='\0';//数组结束标志
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
		 
		  if(UART4_RX_STA<UART4_MAX_RECV_LEN)		//还可以接收数据
		  {
		    TIM_SetCounter(TIM4,0);//计数器清空      
			  if(UART4_RX_STA==0)
					TIM4_Set(1);	 	//使能定时器4的中断
			  UART4_RX_BUF[UART4_RX_STA++]=res;		//记录接收到的值***************************              
		  }
			else 
		  {
			  UART4_RX_STA|=1<<15;					//强制标记接收完成

          }			
			
	 }  											 
}   
//串口4,printf 函数
//确保一次发送数据不超过UART4_MAX_SEND_LEN字节
void u4_printf(char* fmt,...)  
{ 
	u16 len,i;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)UART4_TX_BUF,fmt,ap); //发送
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


//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM4_Init(u16 arr,u16 psc)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);      //时钟使能//TIM4时钟使能  
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);           //优先组设置2
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	TIM_DeInit(TIM4);                                         //复位TIM4定时器 
	TIM_TimeBaseStructure.TIM_Period=arr;                     //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler=psc;                  //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //TIM向上计数模式
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);                     //清除定时器4的溢出标志位
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);           //根据指定的参数初始化TIMx的时间基数单位
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE );                 //使能指定的TIM4中断,允许更新中断
	//TIM_Cmd(TIM4, ENABLE);      
}

//设置TIM4的开关
//sta:0，关闭;1,开启;
void TIM4_Set(u8 sta)
{
	if(sta)
	{  
		TIM_SetCounter(TIM4,0);//计数器清空
		TIM_Cmd(TIM4, ENABLE);  //使能TIMx	
	}
	else 
		TIM_Cmd(TIM4, DISABLE);//关闭定时器4
}

//定时器4中断服务程序		    
void TIM4_IRQHandler(void)
{ 	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)//是更新中断
	{	 			   
		UART4_RX_STA|=1<<15;	//标记接收完成
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //清除TIMx更新中断标志    
		TIM4_Set(0);			//关闭TIM4  
	}   
}


