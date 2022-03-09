#include "delay.h"
#include "wk2xxx.h"
#include "led.h"
//#include "key.h"
#include "sys.h"
#include "usart.h"
#include "wk2xxx_test.h"
/*wk芯片的自发自收测试*/
u8 wk_test1(void)
{
	int i;
	u8 err=0,dat,dat1;
	/*波特率匹配，当主接口使用串口的时候，调用该函数*/
	Wk_BaudAdaptive();//匹配波特率
	delay_ms(100);
	/*读写GNEA，测试主接口通信是否成功*/
	dat1=WkReadGReg(WK2XXX_GENA);
	printf("gena=0x%x.\n",dat1);
	/*根据主接口选择不同，读出的寄存器默认值不一样*/
	WkWriteGReg(WK2XXX_GENA,0x05);
	dat1=WkReadGReg(WK2XXX_GENA);
	printf("gena1=0x%x.\n",dat1);
	
	Wk_Init(1);
	Wk_Init(2);
	Wk_Init(3);
	Wk_Init(4);
	Wk_SetBaud(1,B115200);
	Wk_SetBaud(2,B115200);
	Wk_SetBaud(3,B115200);
	Wk_SetBaud(4,B115200);
	
	
	for(i=0;i<=255;i++)
		{
			WkWriteSReg(1,WK2XXX_FDAT,i);
			WkWriteSReg(2,WK2XXX_FDAT,i);
			WkWriteSReg(3,WK2XXX_FDAT,i);
			WkWriteSReg(4,WK2XXX_FDAT,i);
		}
	for(i=0;i<=255;i++)
		{
			dat=WkReadSReg(1,WK2XXX_FDAT);
				if(dat!=i)
				{ 
					printf("dat1=0x%x.i=0x%x!!\n",dat,i);
					err=1;
			  }
			dat=WkReadSReg(2,WK2XXX_FDAT);
				if(dat!=i)
				{ 
					printf("dat2=0x%x.i=0x%x!!\n",dat,i);
					err=1;
			  }
			dat=WkReadSReg(3,WK2XXX_FDAT);
				if(dat!=i)
				{ 
					printf("dat3=0x%x.i=0x%x!!\n",dat,i);
					err=1;
			  }
			dat=WkReadSReg(4,WK2XXX_FDAT);
				if(dat!=i)
				{ 
					printf("dat4=0x%x.i=0x%x!!\n",dat,i);
					err=1;
			  }
		}
	 if(err)
	 printf("tset fail!!\n");
	 else
	 printf("test success!!\n");
	 return err;
}
/*常用寄存器值的打印*/
void wk_test2(u8 port)
{
	u8 dat1,dat2,dat3,dat4;
	dat1=WkReadGReg(WK2XXX_GENA);
	dat2=WkReadGReg(WK2XXX_GRST);
	dat3=WkReadGReg(WK2XXX_GIER);
	dat4=WkReadGReg(WK2XXX_GIFR);
	printf("GENA=0x%x.GRST=0x%x;GIER=0x%x.GIFR=0x%x.!!\n",dat1,dat2,dat3,dat4);
	dat1=WkReadSReg(port,WK2XXX_SPAGE);
	dat2=WkReadSReg(port,WK2XXX_SCR);
	dat3=WkReadSReg(port,WK2XXX_LCR);
	dat4=WkReadSReg(port,WK2XXX_FCR);
	printf("SPAGE=0x%x.SCR=0x%x;LCR=0x%x.FCR=0x%x.!!\n",dat1,dat2,dat3,dat4);
	dat1=WkReadSReg(port,WK2XXX_SIER);
	dat2=WkReadSReg(port,WK2XXX_SIFR);
	dat3=WkReadSReg(port,WK2XXX_FSR);
	dat4=WkReadSReg(port,WK2XXX_FWCR);
	printf("SIER=0x%x.SIFR=0x%x;FSR=0x%x.FWCR=0x%x.!!\n",dat1,dat2,dat3,dat4);
	
	WkWriteSReg(port,WK2XXX_SPAGE,1);
	dat1=WkReadSReg(port,WK2XXX_BAUD1);
	dat2=WkReadSReg(port,WK2XXX_BAUD0);
	dat3=WkReadSReg(port,WK2XXX_PRES);
	WkWriteSReg(port,WK2XXX_SPAGE,0);
	printf("BAUD1=0x%x.BAUD0=0x%x;PRES=0x%x.!!\n",dat1,dat2,dat3);
		
}
