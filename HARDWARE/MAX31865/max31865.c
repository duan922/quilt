#include "max31865.h"
#include "math.h"
#include "usart.h"
#include "delay.h"
/* MAX31865 初始化 */
void MAX31865_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure ;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	GPIO_InitStructure.GPIO_Pin =MAX31865_SCLK|MAX31865_SDI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MAX31865_CONTROL_PORT,&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin =MAX31865_CS1|MAX31865_CS2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MAX31865_SDO|MAX31865_DRDY;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(MAX31865_CONTROL_PORT,&GPIO_InitStructure);
	
	MAX31865_CS_SET;     //片选拉高
    MAX31865_CS2_SET;     //片选拉高
	MAX31865_SCLK_SET;   //时钟拉高
}


/* MAX31865 写寄存器 
addr:寄存器地址
data:数据
模式0 采集1
模式1 采集2温度
*/
void MAX31865_Write(int a,unsigned char addr, unsigned char data)
{
	unsigned char i;
	if(a==0) MAX31865_CS_CLR;   //片选拉低
    else MAX31865_CS2_CLR;
	for(i=0;i<8;i++)   //写地址
	{
		MAX31865_SCLK_CLR;
		if(addr&0x80) MAX31865_SDI_SET;
		else MAX31865_SDI_CLR;
		MAX31865_SCLK_SET;
		addr<<=1;
	}
	for(i=0;i<8;i++)  //写数据
	{
		MAX31865_SCLK_CLR;
		if(data&0x80) MAX31865_SDI_SET;
		else MAX31865_SDI_CLR;
		MAX31865_SCLK_SET;
		data<<=1;
	}
	if(a==0) MAX31865_CS_SET;
    else MAX31865_CS2_SET;
}

/*
MAX31865 读寄存器 
addr:寄存器地址
*/
unsigned char MAX31865_Read(int b,unsigned char addr)
{
	unsigned char i;
	unsigned char data=0;
	
    if(b==0)MAX31865_CS_CLR;
    else MAX31865_CS2_CLR;
	for(i=0;i<8;i++)  //写地址
	{
		MAX31865_SCLK_CLR;
		if(addr&0x80) MAX31865_SDI_SET;
		else MAX31865_SDI_CLR;
		MAX31865_SCLK_SET;
		addr<<=1;
	}
	for(i=0;i<8;i++)  //读数据
	{
		MAX31865_SCLK_CLR;
		data<<=1;		
		MAX31865_SCLK_SET;		
		if(MAX31865_SDO_READ) data|=0x01;
		else data|=0x00;
	}
	if(b==0) MAX31865_CS_SET;
    else MAX31865_CS2_SET;
	return data;
}

/* MAX31865 配置*/
void MAX31865_Cfg(void)
{
	MAX31865_Write(0,0x80, 0xD3); //BIAS ON,自动，4线，50HZ  根据文件修改四线还是三线
    MAX31865_Write(1,0x80, 0xD3);
}

/* MAX31865 获取温度 */
float MAX31865_GetTemp(int c)
{ 
	unsigned int data;
	float Rt;
	float Rt0 = 100;  //PT100	
	float Z1,Z2,Z3,Z4,temp;
	float a = 3.9083e-3;
	float b = -5.775e-7;
	float rpoly;
if(c==0)
{
//	MAX31865_Write(0x80, 0xD3);
	data=MAX31865_Read(0,0x01)<<8;
	data|=MAX31865_Read(0,0x02);
	data>>=1;  //去掉Fault位
//	printf("Read=0x%02X\r\n",data);
}
else
{
    if(c==1)
    {
        data=MAX31865_Read(1,0x01)<<8;
        data|=MAX31865_Read(1,0x02);
        data>>=1;  //去掉Fault位
    }
}
	Rt=(float)data/32768.0*RREF;
	
//	printf("Rt=0x%.1f\r\n",Rt);
	
  Z1 = -a;
  Z2 = a*a-4*b;
  Z3 = 4*b/Rt0;
  Z4 = 2*b;

  temp = Z2+Z3*Rt;
  temp = (sqrt(temp)+Z1)/Z4;
  
  if(temp>=0) return temp;

  rpoly = Rt;
  temp = -242.02;
  temp += 2.2228 * rpoly;
  rpoly *= Rt;  // square
  temp += 2.5859e-3 * rpoly;
  rpoly *= Rt;  // ^3
  temp -= 4.8260e-6 * rpoly;
  rpoly *= Rt;  // ^4
  temp -= 2.8183e-8 * rpoly;
  rpoly *= Rt;  // ^5
  temp += 1.5243e-10 * rpoly;

  return temp;
}



