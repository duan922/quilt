#define _DHT11_C_
#include <math.h>
#include "DHT11.h"
#include "delay.h"
#include "stdlib.h"

/**
* ����:  DHT11�ɼ�Դ�ļ�
* ˵��:  �ɼ�����ʪ�����ݴ洢--DHT11Data[4]
* ˵��:  None
* ˵��:  None
* ֧��:  QQ946029359 --Ⱥ 607064330
* �Ա�:  https://shop411638453.taobao.com/
* ����:  С��
**/


u8 DHT11Data[4]={0};//��ʪ������(ʪ�ȸ�λ,ʪ�ȵ�λ,�¶ȸ�λ,�¶ȵ�λ)
u8 GatherRrrorCnt = 0;//�ɼ�����Ĵ���
u8 LastT=0,LastR=0;//��¼��һ�ε���ʪ��
u32 RendTHCnt = 0;//��ʱ�ɼ���ʪ������
   
/**
* @brief  GPIO
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void DHT11_GPIO_Init(void)	   
{                 
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//��©��� 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					     //�����趨������ʼ��
}

/**
* @brief  DHT11��ʼ�ź�
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void DHT11_Rst(void)	   
{                 
	DHT11_DQ_OUT=0; 	
	delay_ms(20);    	
	DHT11_DQ_OUT=1; 	
	delay_us(30);   
}



u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
  while (DHT11_DQ_IN&&retry<100){
		retry++;
		delay_us(1);
	} 
	
	if(retry>=100){
    return 1;
	}
	
	retry=0;
  while (!DHT11_DQ_IN&&retry<100){
		retry++;
		delay_us(1);
	}
	
	if(retry>=100){
		return 1;	 
	}   
	return 0;
}



u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_DQ_IN&&retry<100)
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)
	{
		retry++;
		delay_us(1);
	}
	delay_us(35);
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}

u8 DHT11_Read_Byte(void)    
{        
	u8 i,dat;
	dat=0;
	for (i=0;i<8;i++) {
	dat<<=1; 
	dat|=DHT11_Read_Bit();
	}						    
	return dat;
}

void DHT11_Read_Data(void)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++){
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4]){
			DHT11Data[0]=buf[0];
			DHT11Data[1]=buf[1];
			DHT11Data[2]=buf[2];
			DHT11Data[3]=buf[3];
			LastT = DHT11Data[2];
			LastR = DHT11Data[0];
		}
		else{
			if(abs(LastT-DHT11Data[2])<3 && abs(LastR-DHT11Data[0])<3)//������Ǻܴ�Ҳ��Ϊ����ȷ������
			{
				DHT11Data[0]=buf[0];
				DHT11Data[1]=buf[1];
				DHT11Data[2]=buf[2];
				DHT11Data[3]=buf[3];
				GatherRrrorCnt = 0;
      }
			else
			{
        GatherRrrorCnt++;
      }
    }
		if(GatherRrrorCnt>5)//����5�βɼ�����
		{
			GatherRrrorCnt = 0;
			DHT11Data[0]=0xff;
			DHT11Data[1]=0xff;
			DHT11Data[2]=0xff;
			DHT11Data[3]=0xff;
		}
	}
}
