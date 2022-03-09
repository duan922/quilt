#ifndef __USART_H
#define __USART_H
#include <stm32f10x.h>
#ifndef USART_C_//如果没有定义
#define USART_Ex_ extern
#else
#define USART_Ex_
#endif


#include "BufferManage.h"
#include "LoopList.h"

/****************************串口1****************************/
/*
串口1接收数据:缓存管理
串口1发送数据:环形队列+中断
*/
USART_Ex_ buff_manage_struct buff_manage_struct_usart1_read;//串口接收缓存管理
USART_Ex_ rb_t rb_t_usart1_send;//串口1发送缓存环形队列

#define Usart1ReadBuffLen 512  //串口1接收数据数组大小
#define Usart1ReadBufferMemoryLen 1024 //串口接收缓存区大小
#define Usart1ReadBufferMemoryCopyLen 512 //提取缓存区的数据的数组大小
#define Usart1ReadBufferMemoryManageLen 50 //串口接收缓存区管理
#define Usart1LoopListSendBuffLen 1024  //串口1发送数据缓存大小

USART_Ex_ unsigned char Usart1ReadBuff[Usart1ReadBuffLen];  //接收数据缓存
USART_Ex_ u32  Usart1ReadCnt;//串口1接收到的数据个数

USART_Ex_ u8   Usart1ReadBufferMemory[Usart1ReadBufferMemoryLen];//接收数据缓存区
USART_Ex_ u32  Usart1ReadBufferMemoryManage[Usart1ReadBufferMemoryManageLen];//管理缓存区
USART_Ex_ u8   Usart1ReadBufferMemoryCopy[Usart1ReadBufferMemoryCopyLen];//提取缓存区的数据的数组

USART_Ex_ unsigned char Usart1LoopListSendBuff[Usart1LoopListSendBuffLen];  //串口1发送数据缓存

/****************************串口2****************************/
/*
串口2接收数据:缓存管理
串口2发送数据:普通中断
*/
USART_Ex_ buff_manage_struct buff_manage_struct_usart3_read;//串口接收缓存管理

#define Usart3ReadBuffLen 512 //接收数据缓存
#define Usart3BufferMemoryLen 1024 //接收数据缓存区
#define Usart3BufferMemoryCopyLen 512 //提取缓存区的数据
#define Usart3BufferMemoryManageLen 50 //管理缓存区

USART_Ex_ unsigned char Usart3ReadBuff[Usart3ReadBuffLen];  //接收数据缓存
USART_Ex_ u32  Usart3ReadCnt;//串口接收到的数据个数

USART_Ex_ u8   Usart3BufferMemory[Usart3BufferMemoryLen];//接收数据缓存区
USART_Ex_ u8   Usart3BufferMemoryCopy[Usart3BufferMemoryCopyLen];//提取缓存区的数据
USART_Ex_ u32  Usart3BufferMemoryManage[Usart3BufferMemoryManageLen];//管理缓存区

USART_Ex_ unsigned char *Usart3SendData;//串口中断发送数据的地址
USART_Ex_ u32   Usart3SendDataCnt;//串口中断发送数据的个数


void UsartOutStrIT(USART_TypeDef* USARTx, unsigned char *c,uint32_t cnt);
void uart_init(u32 bound1,u32 bound2);//串口初始化
void UsartOutStr(USART_TypeDef* USARTx, unsigned char *c,uint32_t cnt);//串口发送字符串数据
void UsartOutChar(USART_TypeDef* USARTx, unsigned char c);//串口发送一个字节
#endif


