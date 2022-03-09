#ifndef __UART4_H
#define __UART4_H
#include "stdio.h"	
#include "sys.h" 


extern u8 tcp_rx[255];
#define UART4_MAX_RECV_LEN		1024				//最大接收缓存字节数
#define UART4_MAX_SEND_LEN		256				  //最大发送缓存字节数
extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  UART4_TX_BUF[UART4_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern u16 UART4_RX_STA;   						//接收数据状态
extern char  GET_num; 
void uart4_init(u32 bound);
void TIM4_Set(u8 sta);
void TIM4_Init(u16 arr,u16 psc);
void u4_printf(char* fmt, ...);


#endif


