#include "wk2xxx.h"
#include "delay.h"
#include "uart5.h"


/**************************************Wk2114WriteReg***********************************/
//�������ܣ�д�Ĵ���������ǰ���ǸüĴ�����д��ĳЩ�Ĵ��������д1�����ܻ��Զ���1������������ֲ�)
//������port:Ϊ�Ӵ��ڵ���(C0C1)
//      reg:Ϊ�Ĵ����ĵ�ַ(A3A2A1A0)
//      dat:Ϊд��Ĵ���������
//ע�⣺���Ӵ��ڱ���ͨ������£���FDATд������ݻ�ͨ��TX�������
//*************************************************************************/
void Wk2114WriteReg(unsigned char port,unsigned char reg,unsigned char dat)
{	 
	 UART5_Send_Byte(((port-1)<<4)+reg);	//дָ�����ָ��Ĺ��ɼ������ֲ�
	 UART5_Send_Byte(dat);//д����
}


/*************************************Wk2114ReadReg************************************/
//�������ܣ����Ĵ�������
//������port:Ϊ�Ӵ��ڵ���(C0C1)
//      reg:Ϊ�Ĵ����ĵ�ַ(A3A2A1A0)
//      rec_data:Ϊ��ȡ���ļĴ���ֵ
//ע�⣺���Ӵ��ڱ���ͨ������£���FDAT��ʵ���Ͼ��Ƕ�ȡuart��rx���յ�����
/*************************************************************************/
unsigned char Wk2114ReadReg(unsigned char port,unsigned char reg)
{	 
    unsigned char rec_data;
    UART5_Send_Byte(0x40+((port-1)<<4)+reg); //дָ�����ָ��Ĺ��ɼ������ֲ�
	  rec_data=uart5_recByte();	 //���շ��صļĴ���ֵ									
	  return rec_data;
}



/**************************************Wk2114writeFIFO***********************************/
//�������ܣ�дFIFO�������ú���д������ݻ�ͨ��uart��TX���ͳ�ȥ)
//������port:Ϊ�Ӵ��ڵ���(C0C1)
//      *dat��Ϊд������ָ��
//      num��Ϊд�����ݵĸ�����������16���ֽڣ�N3N2N1N0��
/*************************************************************************/
//void Wk2114writeFIFO(unsigned char port,unsigned char *send_da,unsigned char num)
//{	 
//	 unsigned char i;
//	 uart_sendByte(0x80+((port-1)<<4)+(num-1)); //дָ��,����ָ��ɼ������ֲ�
//	 for(i=0;i<num;i++)
//	 {
//	   uart_sendByte( *(send_da+i));//д����
//	 }
//}

/***************************************Wk2114readFIFO**********************************/
//�������ܣ���FIFO�������ú�����ȡ��������FIFO�����е����ݣ�ʵ������uart��rx���յ�����)
//������port:Ϊ�Ӵ��ڵ���(C0C1)
//      *dat��Ϊ��������ָ��
//      num��Ϊ�������ݵĸ�����������16���ֽڣ�N3N2N1N0��
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
//�������ܣ���������Ҫ���ʼ��һЩоƬ�����Ĵ�����
/*********************************************************************************/
void Wk2114Init(unsigned char port)
{
    unsigned char gena,grst,gier,sier,scr;
	//ʹ���Ӵ���ʱ��
    gena=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GENA); 
	switch (port)
    {
          case 1://ʹ���Ӵ���1��ʱ��
              gena|=WK2XXX_UT1EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		  case 2://ʹ���Ӵ���2��ʱ��
              gena|=WK2XXX_UT2EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 3://ʹ���Ӵ���3��ʱ��
              gena|=WK2XXX_UT3EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 4://ʹ���Ӵ���4��ʱ��
              gena|=WK2XXX_UT4EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
	 }	
	//�����λ�Ӵ���
	grst=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GRST); 
	switch (port)
    {
          case 1://�����λ�Ӵ���1
              grst|=WK2XXX_UT1RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		  case 2://�����λ�Ӵ���2
              grst|=WK2XXX_UT2RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 3://�����λ�Ӵ���3
              grst|=WK2XXX_UT3RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 4://�����λ�Ӵ���4
             grst|=WK2XXX_UT4RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
	 }	
  //ʹ���Ӵ����жϣ������Ӵ������жϺ��Ӵ����ڲ��Ľ����жϣ��������жϴ���
	gier=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GIER); 
	switch (port)
    {
          case 1://�����λ�Ӵ���1
              gier|=WK2XXX_UT1RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		  case 2://�����λ�Ӵ���2
              gier|=WK2XXX_UT2RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		   case 3://�����λ�Ӵ���3
              gier|=WK2XXX_UT3RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		   case 4://�����λ�Ӵ���4
              gier|=WK2XXX_UT4RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
	 }	 
	 //ʹ���Ӵ��ڽ��մ����жϺͳ�ʱ�ж�
	 sier=Wk2114ReadReg(port,WK2XXX_SIER); 
	 sier |= WK2XXX_RFTRIG_IEN|WK2XXX_RXOUT_IEN;
	 Wk2114WriteReg(port,WK2XXX_SIER,sier);
	 // ��ʼ��FIFO�����ù̶��жϴ���
	 Wk2114WriteReg(port,WK2XXX_FCR,0XFF);
	 //���������жϴ��㣬��������������Ч����ô����FCR�Ĵ����жϵĹ̶��жϴ��㽫ʧЧ
	 Wk2114WriteReg(port,WK2XXX_SPAGE,1);//�л���page1
	 Wk2114WriteReg(port,WK2XXX_RFTL,0X40);//���ý��մ���Ϊ64���ֽ�
	 Wk2114WriteReg(port,WK2XXX_TFTL,0X10);//���÷��ʹ���Ϊ16���ֽ�
	 Wk2114WriteReg(port,WK2XXX_SPAGE,0);//�л���page0 
	 //ʹ���Ӵ��ڵķ��ͺͽ���ʹ��
	 scr=Wk2114ReadReg(port,WK2XXX_SCR); 
	 scr|=WK2XXX_TXEN|WK2XXX_RXEN;
	 Wk2114WriteReg(port,WK2XXX_SCR,scr);
}
/******************************Wk2114Close*******************************************/
//�������ܣ���������رյ�ǰ�Ӵ��ڣ��͸�λ��ʼֵ��
/*********************************************************************************/

void Wk2114Close(unsigned char port)
{
    unsigned char gena,grst;
	//��λ�Ӵ���
	grst=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GRST); 
	switch (port)
    {
          case 1://�����λ�Ӵ���1
              grst&=~WK2XXX_UT1RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		  case 2://�����λ�Ӵ���2
              grst&=~WK2XXX_UT2RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 3://�����λ�Ӵ���3
              grst&=~WK2XXX_UT3RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 4://�����λ�Ӵ���4
              grst&=~WK2XXX_UT4RST;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
	 }	
	//�ر��Ӵ���ʱ��
    gena=Wk2114ReadReg(WK2XXX_GPORT,WK2XXX_GENA); 
	switch (port)
    {
          case 1://ʹ���Ӵ���1��ʱ��
              gena&=~WK2XXX_UT1EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		  case 2://ʹ���Ӵ���2��ʱ��
              gena&=~WK2XXX_UT2EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 3://ʹ���Ӵ���3��ʱ��
              gena&=~WK2XXX_UT3EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 4://ʹ���Ӵ���4��ʱ��
              gena&=~WK2XXX_UT4EN;
		      Wk2114WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
	 }	
}
/**************************Wk2114SetBaud*******************************************************/
//�������ܣ������Ӵ��ڲ����ʺ������˺����в����ʵ�ƥ��ֵ�Ǹ���11.0592Mhz�µ��ⲿ��������
// port:�Ӵ��ں�
// baud:�����ʴ�С.�����ʱ�ʾ��ʽ��
//
/**************************Wk2114SetBaud*******************************************************/
void Wk2114SetBaud(unsigned char port,int baud)
{  
	unsigned char baud1,baud0,pres,scr;
	//���²�������Ӧ�ļĴ���ֵ�������ⲿʱ��Ϊ11.0592������¼������ã����ʹ������������Ҫ���¼���
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
	//�ص��Ӵ����շ�ʹ��
	scr=Wk2114ReadReg(port,WK2XXX_SCR); 
	Wk2114WriteReg(port,WK2XXX_SCR,0);
	//���ò�������ؼĴ���
	Wk2114WriteReg(port,WK2XXX_SPAGE,1);//�л���page1
	Wk2114WriteReg(port,WK2XXX_BAUD1,baud1);
	Wk2114WriteReg(port,WK2XXX_BAUD0,baud0);
	Wk2114WriteReg(port,WK2XXX_PRES,pres);
	Wk2114WriteReg(port,WK2XXX_SPAGE,0);//�л���page0 
	//ʹ���Ӵ����շ�ʹ��
	Wk2114WriteReg(port,WK2XXX_SCR,scr);
	
	
}



