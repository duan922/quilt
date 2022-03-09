#ifndef __USART_H
#define __USART_H
#include <stm32f10x.h>
#ifndef USART_C_//���û�ж���
#define USART_Ex_ extern
#else
#define USART_Ex_
#endif


#include "BufferManage.h"
#include "LoopList.h"

/****************************����1****************************/
/*
����1��������:�������
����1��������:���ζ���+�ж�
*/
USART_Ex_ buff_manage_struct buff_manage_struct_usart1_read;//���ڽ��ջ������
USART_Ex_ rb_t rb_t_usart1_send;//����1���ͻ��滷�ζ���

#define Usart1ReadBuffLen 512  //����1�������������С
#define Usart1ReadBufferMemoryLen 1024 //���ڽ��ջ�������С
#define Usart1ReadBufferMemoryCopyLen 512 //��ȡ�����������ݵ������С
#define Usart1ReadBufferMemoryManageLen 50 //���ڽ��ջ���������
#define Usart1LoopListSendBuffLen 1024  //����1�������ݻ����С

USART_Ex_ unsigned char Usart1ReadBuff[Usart1ReadBuffLen];  //�������ݻ���
USART_Ex_ u32  Usart1ReadCnt;//����1���յ������ݸ���

USART_Ex_ u8   Usart1ReadBufferMemory[Usart1ReadBufferMemoryLen];//�������ݻ�����
USART_Ex_ u32  Usart1ReadBufferMemoryManage[Usart1ReadBufferMemoryManageLen];//��������
USART_Ex_ u8   Usart1ReadBufferMemoryCopy[Usart1ReadBufferMemoryCopyLen];//��ȡ�����������ݵ�����

USART_Ex_ unsigned char Usart1LoopListSendBuff[Usart1LoopListSendBuffLen];  //����1�������ݻ���

/****************************����2****************************/
/*
����2��������:�������
����2��������:��ͨ�ж�
*/
USART_Ex_ buff_manage_struct buff_manage_struct_usart3_read;//���ڽ��ջ������

#define Usart3ReadBuffLen 512 //�������ݻ���
#define Usart3BufferMemoryLen 1024 //�������ݻ�����
#define Usart3BufferMemoryCopyLen 512 //��ȡ������������
#define Usart3BufferMemoryManageLen 50 //��������

USART_Ex_ unsigned char Usart3ReadBuff[Usart3ReadBuffLen];  //�������ݻ���
USART_Ex_ u32  Usart3ReadCnt;//���ڽ��յ������ݸ���

USART_Ex_ u8   Usart3BufferMemory[Usart3BufferMemoryLen];//�������ݻ�����
USART_Ex_ u8   Usart3BufferMemoryCopy[Usart3BufferMemoryCopyLen];//��ȡ������������
USART_Ex_ u32  Usart3BufferMemoryManage[Usart3BufferMemoryManageLen];//��������

USART_Ex_ unsigned char *Usart3SendData;//�����жϷ������ݵĵ�ַ
USART_Ex_ u32   Usart3SendDataCnt;//�����жϷ������ݵĸ���


void UsartOutStrIT(USART_TypeDef* USARTx, unsigned char *c,uint32_t cnt);
void uart_init(u32 bound1,u32 bound2);//���ڳ�ʼ��
void UsartOutStr(USART_TypeDef* USARTx, unsigned char *c,uint32_t cnt);//���ڷ����ַ�������
void UsartOutChar(USART_TypeDef* USARTx, unsigned char c);//���ڷ���һ���ֽ�
#endif


