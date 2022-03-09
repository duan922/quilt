#include "wk2xxx.h"
#include "delay.h"
#include "uart5.h"


/**************************************Wk2114WriteReg***********************************/
//函数功能：写寄存器函数（前提是该寄存器可写，某些寄存器如果你写1，可能会自动置1，具体见数据手册)
//参数：port:为子串口的数(C0C1)
//      reg:为寄存器的地址(A3A2A1A0)
//      dat:为写入寄存器的数据
//注意：在子串口被打通的情况下，向FDAT写入的数据会通过TX引脚输出
//*************************************************************************/
void Wk2114WriteReg(unsigned char port,unsigned char reg,unsigned char dat)
{	 
	 UART5_Send_Byte(((port-1)<<4)+reg);	//写指令，对于指令的构成见数据手册
	 UART5_Send_Byte(dat);//写数据
}


/*************************************Wk2114ReadReg************************************/
//函数功能：读寄存器函数
//参数：port:为子串口的数(C0C1)
//      reg:为寄存器的地址(A3A2A1A0)
//      rec_data:为读取到的寄存器值
//注意：在子串口被打通的情况下，读FDAT，实际上就是读取uart的rx接收的数据
/*************************************************************************/
unsigned char Wk2114ReadReg(unsigned char port,unsigned char reg)
{	 
    unsigned char rec_data;
    UART5_Send_Byte(0x40+((port-1)<<4)+reg); //写指令，对于指令的构成见数据手册
	  rec_data=uart5_recByte();	 //接收返回的寄存器值									
	  return rec_data;
}



/**************************************Wk2114writeFIFO***********************************/
//函数功能：写FIFO函数（该函数写入的数据会通过uart的TX发送出去)
//参数：port:为子串口的数(C0C1)
//      *dat：为写入数据指针
//      num：为写入数据的个数，不超过16个字节（N3N2N1N0）
/*************************************************************************/
//void Wk2114writeFIFO(unsigned char port,unsigned char *send_da,unsigned char num)
//{	 
//	 unsigned char i;
//	 uart_sendByte(0x80+((port-1)<<4)+(num-1)); //写指令,对于指令构成见数据手册
//	 for(i=0;i<num;i++)
//	 {
//	   uart_sendByte( *(send_da+i));//写数据
//	 }
//}

/***************************************Wk2114readFIFO**********************************/
//函数功能：读FIFO函数（该函数读取的数据是FIFO缓存中的数据，实际上是uart的rx接收的数据)
//参数：port:为子串口的数(C0C1)
//      *dat：为读到数据指针
//      num：为读出数据的个数，不超过16个字节（N3N2N1N0）
/*************************************************************************/
//unsigned char *Wk2114readFIFO(unsigned char port,unsigned char num)
//{
//		unsigned char n;
//		unsigned char dat[256];
//		uart_sendByte(0xc0+((port-1)<<4)+(num-1));
//		for(n=0;n<num;n++)
//		{	
//				dat[n]=uart_recByte();	
//		}
//		return dat;
//}

/******************************Wk2114Init*******************************************/
//函数功能：本函数主要会初始化一些芯片基本寄存器；
/*********************************************************************************/
void Wk2114Init(unsigned char port)
{
    unsigned char gena,grst,gier,sier,scr;
	//使能子串口时钟
    gena=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GENA); 
	switch (port)
    {
          case 1://使能子串口1的时钟
              gena|=WK2XXX_UT1EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		  case 2://使能子串口2的时钟
              gena|=WK2XXX_UT2EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 3://使能子串口3的时钟
              gena|=WK2XXX_UT3EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 4://使能子串口4的时钟
              gena|=WK2XXX_UT4EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
	 }	
	//软件复位子串口
	grst=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GRST); 
	switch (port)
    {
          case 1://软件复位子串口1
              grst|=WK2XXX_UT1RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		  case 2://软件复位子串口2
              grst|=WK2XXX_UT2RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 3://软件复位子串口3
              grst|=WK2XXX_UT3RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 4://软件复位子串口4
             grst|=WK2XXX_UT4RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
	 }	
  //使能子串口中断，包括子串口总中断和子串口内部的接收中断，和设置中断触点
	gier=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GIER); 
	switch (port)
    {
          case 1://软件复位子串口1
              gier|=WK2XXX_UT1RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		  case 2://软件复位子串口2
              gier|=WK2XXX_UT2RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		   case 3://软件复位子串口3
              gier|=WK2XXX_UT3RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		   case 4://软件复位子串口4
              gier|=WK2XXX_UT4RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
	 }	 
	 //使能子串口接收触点中断和超时中断
	 sier=Wk2114ReadReg(port,WK2XXX_SIER); 
	 sier |= WK2XXX_RFTRIG_IEN|WK2XXX_RXOUT_IEN;
	 Wk2114WriteReg(port,WK2XXX_SIER,sier);
	 // 初始化FIFO和设置固定中断触点
	 Wk2114WriteReg(port,WK2XXX_FCR,0XFF);
	 //设置任意中断触点，如果下面的设置有效，那么上面FCR寄存器中断的固定中断触点将失效
	 Wk2114WriteReg(port,WK2XXX_SPAGE,1);//切换到page1
	 Wk2114WriteReg(port,WK2XXX_RFTL,0X40);//设置接收触点为64个字节
	 Wk2114WriteReg(port,WK2XXX_TFTL,0X10);//设置发送触点为16个字节
	 Wk2114WriteReg(port,WK2XXX_SPAGE,0);//切换到page0 
	 //使能子串口的发送和接收使能
	 scr=Wk2114ReadReg(port,WK2XXX_SCR); 
	 scr|=WK2XXX_TXEN|WK2XXX_RXEN;
	 Wk2114WriteReg(port,WK2XXX_SCR,scr);
}
/******************************Wk2114Close*******************************************/
//函数功能：本函数会关闭当前子串口，和复位初始值；
/*********************************************************************************/

void Wk2114Close(unsigned char port)
{
    unsigned char gena,grst;
	//复位子串口
	grst=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GRST); 
	switch (port)
    {
          case 1://软件复位子串口1
              grst&=~WK2XXX_UT1RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		  case 2://软件复位子串口2
              grst&=~WK2XXX_UT2RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 3://软件复位子串口3
              grst&=~WK2XXX_UT3RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 4://软件复位子串口4
              grst&=~WK2XXX_UT4RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
	 }	
	//关闭子串口时钟
    gena=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GENA); 
	switch (port)
    {
          case 1://使能子串口1的时钟
              gena&=~WK2XXX_UT1EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		  case 2://使能子串口2的时钟
              gena&=~WK2XXX_UT2EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 3://使能子串口3的时钟
              gena&=~WK2XXX_UT3EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 4://使能子串口4的时钟
              gena&=~WK2XXX_UT4EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
	 }	
}
/**************************Wk2114SetBaud*******************************************************/
//函数功能：设置子串口波特率函数、此函数中波特率的匹配值是根据11.0592Mhz下的外部晶振计算的
// port:子串口号
// baud:波特率大小.波特率表示方式，
//
/**************************Wk2114SetBaud*******************************************************/
void Wk2114SetBaud(unsigned char port,int baud)
{  
	unsigned char baud1,baud0,pres,scr;
	//如下波特率相应的寄存器值，是在外部时钟为11.0592的情况下计算所得，如果使用其他晶振，需要重新计算
	switch (baud) 
	{
        case 600:
			baud1=0x4;
			baud0=0x7f;
			pres=0;
            break;
        case 1200:
			baud1=0x2;
			baud0=0x3F;
			pres=0;
			break;
        case 2400:
			baud1=0x1;
			baud0=0x1f;
			pres=0;
			break;
        case 4800:
			baud1=0x00;
			baud0=0x8f;
			pres=0;
			break;
        case 9600:
			baud1=0x00;
			baud0=0x47;
			pres=0;
			break;
        case 19200:
			baud1=0x00;
			baud0=0x23;
			pres=0;
			break;
        case 38400:
			baud1=0x00;
			baud0=0x11;
			pres=0;
			break;
        case 76800:
			baud1=0x00;
			baud0=0x08;
			pres=0;
			break; 
       
        case 1800:
			baud1=0x01;
			baud0=0x7f;
			pres=0;
			break;
        case 3600:
			baud1=0x00;
			baud0=0xbf;
			pres=0;
			break;
        case 7200:
			baud1=0x00;
			baud0=0x5f;
			pres=0;
			break;
        case 14400:
			baud1=0x00;
			baud0=0x2f;
			pres=0;
			break;
        case 28800:
			baud1=0x00;
			baud0=0x17;
			pres=0;
			break;
        case 57600:
			baud1=0x00;
			baud0=0x0b;
			pres=0;
            break;
        case 115200:
			baud1=0x00;
			baud0=0x05;
			pres=0;
			break;
        case 230400:
			baud1=0x00;
			baud0=0x02;
			pres=0;
			break;
        default:
			baud1=0x00;
			baud0=0x00;
			pres=0;
    }
	//关掉子串口收发使能
	scr=Wk2114ReadReg(port,WK2XXX_SCR); 
	Wk2114WriteReg(port,WK2XXX_SCR,0);
	//设置波特率相关寄存器
	Wk2114WriteReg(port,WK2XXX_SPAGE,1);//切换到page1
	Wk2114WriteReg(port,WK2XXX_BAUD1,baud1);
	Wk2114WriteReg(port,WK2XXX_BAUD0,baud0);
	Wk2114WriteReg(port,WK2XXX_PRES,pres);
	Wk2114WriteReg(port,WK2XXX_SPAGE,0);//切换到page0 
	//使能子串口收发使能
	Wk2114WriteReg(port,WK2XXX_SCR,scr);
	
	
}



