#ifndef BUFFMANAGE_H_
#define BUFFMANAGE_H_

#ifndef BUFFMANAGE_C_//���û�ж���
#define BUFFMANAGE_Cx_ extern
#else
#define BUFFMANAGE_Cx_
#endif

#include <stm32f10x.h>
#include "LoopList.h"


typedef struct{
	signed int  Count;
	signed int  Cnt;
	unsigned char ReadFlage;
	unsigned char SendFlage;
	
	signed int  ReadLen;
	signed int  SendLen;	//�û�������ʹ�����ϱ���
		
	int32_t value; //�ڲ�ʹ��,�û�����ʹ��
	signed int  Len;	//�ڲ�ʹ��,�û�����ʹ��
	rb_t Buff;        //����:������������
	rb_t ManageBuff;  //����:ÿ�λ������ݵ�����
}buff_manage_struct;



void BufferManageCreate(buff_manage_struct *bms,void *buff,uint32_t BuffLen,void *ManageBuff,uint32_t ManageBuffLen);
void BufferManageWrite(buff_manage_struct *bms,void *buff,uint32_t BuffLen,int *DataLen);
void BufferManageRead(buff_manage_struct *bms,void *buff,int *DataLen);

#endif

