#ifndef __UART5_H
#define __UART5_H
#include "stdio.h"	
#include "System.h" 

void uart5_init(u32 boud);
void UART5_Send_Byte(u8 Data) ;//����һ���ֽڣ�
void UART5_Send_String(u8 *Data); //�����ַ�����
unsigned char uart5_recByte(void) ;

#endif


