#ifndef __UART4_H
#define __UART4_H
#include "stdio.h"	
#include "sys.h" 


extern u8 tcp_rx[255];
#define UART4_MAX_RECV_LEN		1024				//�����ջ����ֽ���
#define UART4_MAX_SEND_LEN		256				  //����ͻ����ֽ���
extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  UART4_TX_BUF[UART4_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern u16 UART4_RX_STA;   						//��������״̬
extern char  GET_num; 
void uart4_init(u32 bound);
void TIM4_Set(u8 sta);
void TIM4_Init(u16 arr,u16 psc);
void u4_printf(char* fmt, ...);


#endif


