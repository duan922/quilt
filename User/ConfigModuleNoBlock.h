#ifndef CONFIGMODULENOBLOCK_H_
#define CONFIGMODULENOBLOCK_H_
#include "stm32f10x.h"
#ifndef CONFIGMODULENOBLOCK_C_
#define CONFIGMODULENOBLOCK_Ex_  extern
#else 
#define CONFIGMODULENOBLOCK_Ex_
#endif

CONFIGMODULENOBLOCK_Ex_ u32 ConfigModuleNoBlockTimerCnt;   //���ú�����ʱ����
CONFIGMODULENOBLOCK_Ex_ char ConfigModuleNoBlockFlage;     //1-������ 0-δ������
CONFIGMODULENOBLOCK_Ex_ int  ConfigModuleNoBlockCaseValue; //����ִ����һ��Case ���

CONFIGMODULENOBLOCK_Ex_ void (*ConfigConnectDispose)(char *data);//����һ������ָ�����,��������ģ�鷵�ص�����

void ConfigModuleNoBlock(void);

void FunctionParseCompare(char *data);//�Ա��ж��Ƿ񷵻���Ҫ������
void SendConfigFunction(char *order,void (*FunctionSend)(),char *HopeReturn1,char *HopeReturn2,void (*FunctionParse)(char *data),u32 ConfigFunctionValue);


#endif

