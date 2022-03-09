#ifndef __UART5_H
#define __UART5_H
#include "stdio.h"	
#include "System.h" 

void uart5_init(u32 boud);
void UART5_Send_Byte(u8 Data) ;//发送一个字节；
void UART5_Send_String(u8 *Data); //发送字符串；
unsigned char uart5_recByte(void) ;

#endif


