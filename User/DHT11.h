#ifndef __DHT11_H_
#define __DHT11_H_

#ifndef _DHT11_C_
#define _DHT11_C_ extern
#else
#define _DHT11_C_
#endif
#include <stm32f10x.h>
#include "System.h"
  
										   
#define	DHT11_DQ_OUT PBout(9) 
#define	DHT11_DQ_IN  PBin(9)

_DHT11_C_ u8 DHT11Data[4];//��ʪ������(�¶ȸ�λ,�¶ȵ�λ,ʪ�ȸ�λ,ʪ�ȵ�λ)
_DHT11_C_ u32 RendTHCnt;//��ʱ�ɼ���ʪ������


void DHT11_GPIO_Init(void);
void DHT11_Read_Data(void); 
u8 DHT11_Read_Byte(void);//??????
u8 DHT11_Read_Bit(void);//?????
u8 DHT11_Check(void);//??????DHT11
void DHT11_Rst(void);//??DHT11  



#endif

