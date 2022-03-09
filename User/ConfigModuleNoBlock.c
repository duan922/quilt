/**
  ******************************************************************************
  * @file    usart.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   
  ******************************************************************************
	һ,ʹ��˵��
		1,�����³������1ms��ʱ���ж���
			ConfigModuleNoBlockTimerCnt++;
			
		2,�����³���ŵ�������
		while(1)
		{
			ConfigModuleNoBlock();//����ģ��
			
			if(���ڽ��յ�һ������������)
			{
			  ConfigConnectDispose(���յ����ݵ�ַ);
			}
		}
			
  ******************************************************************************
  */
//**********����ģ��


#define CONFIGMODULENOBLOCK_C_
#include <stdio.h>

#include "ConfigModuleNoBlock.h"
#include "main.h"
#include "cString.h"
#include "usart.h"
#include "mqtt.h"
#include "System.h"


u32 ConfigModuleNoBlockTimerCnt =0;   //���ú�����ʱ����,��ʱ���ڲ��ۼ�
int ConfigModuleNoBlockCaseValue = 0; //����ִ����һ��Case ���
char ConfigModuleNoBlockFlage  = 0;   //1-���������� 0-δ����������

u32 CompareValue=5000;                //ÿ�� Ms ����һ������
u32 SendNextDelay =0;                 //����SendConfigFunction�������һ������,���մ��ݸ� ConfigModuleNoBlockCnt ����дһ�����ݷ��͵�ʱ��
int Cnt = 0;                          //��¼����״̬���͵Ĵ���
char DataReturnFlage  = 0;            //�Ƿ񷵻���Ԥ�ڵ�����

char HopeReturnData1[20]="";//�洢ϣ�����ص�����
char HopeReturnData2[20]="";//�洢ϣ�����ص�����

void (*ConfigConnectDispose)(char *data);//����һ������ָ�����,��������ģ�鷵�ص�����

/*********************************************************************************************************/



/**
* @brief  //��ȡΨһID,WIFI��ȡMAC,GPRS��ȡIMEI
* @param  data  
* @param  
* @retval 
* @example 
**/
void FunctionParseGetID(char *data)
{
	MainString=StrBetwString(data,"\r\n","\r\n");
	
	if(strlen(MainString) ==15)
	{
		memset(MQTTid,0,sizeof(MQTTid));
		memcpy(MQTTid,MainString,15);
		//�����ʹ���Զ���ID,��������������
		
	  DataReturnFlage=1;
		ConfigModuleNoBlockTimerCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockTimerCnt:SendNextDelay);
	}
	cStringRestore();
}


/**
* @brief  //���MQTTЭ��,������AT+CIPSEND=XXX
* @param  
* @param 
* @retval 
* @example 
**/
void FunctionParseConnectMqtt(void)
{		
	unsigned char *str;
	int len;
	memset(MQTTPublishTopic,NULL,sizeof(MQTTPublishTopic));
	sprintf(MQTTPublishTopic,"%s%s","device/",&MQTTid[0]);//��Ϸ���������
	
	
	mymqtt.mqtt_connect_info.client_id = MQTTid;//client_id
	mymqtt.mqtt_connect_info.keepalive = MQTTkeepAlive;//������ʱ��
	mymqtt.mqtt_connect_info.username = MQTTUserName;//�û���
	mymqtt.mqtt_connect_info.password = MQTTPassWord;///����
	
	
	mymqtt.mqtt_connect_info.will_topic = MQTTPublishTopic;//��������������
	mymqtt.mqtt_connect_info.will_message = MQTTWillMessage;//��������Ϣ
	mymqtt.mqtt_connect_info.will_qos = MQTTWillQos;//��������Ϣ�ȼ�
	mymqtt.mqtt_connect_info.will_retain = MQTTWillRetained;//�Ƿ���Ҫ������������Ϣ
	mymqtt.mqtt_connect_info.clean_session = 1;//���������Ϣ
	
	len = mqtt_connect(&mymqtt,&str);//���������Ϣ
	
	if(len>0)
        {
		UsartOutStrIT(USART3,str,len);//����MQTTЭ������
	}
}


/**
* @brief  //�ж��Ƿ�������MQTT
* @param  data:MQTT���������ص�����
* @param 
* @retval 
* @example 
**/
void FunctionParseConnectMqttAck(char *data)
{
	if(mqtt_connect_ack(data)==0)
	{
		DataReturnFlage=1;
		ConfigModuleNoBlockTimerCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockTimerCnt:SendNextDelay);
		
		if(mymqtt.connectCb){//�������ӻص�����
			mymqtt.connectCb();
		}
	}
}


/**
* @brief  ����ָ������ģ��,��������
* @waring 
* @param  None 
* @param  None
* @param  None
* @retval None
* @example 
**/
void ConfigModuleNoBlock(void)
{
	if(ConfigModuleNoBlockTimerCnt>CompareValue && ConfigModuleNoBlockFlage==0)
	{
		ConfigModuleNoBlockTimerCnt=0;
		if(DataReturnFlage == 1)//��һ��ָ����OK��
		{
			Cnt=0;
			DataReturnFlage = 0;
			ConfigModuleNoBlockCaseValue ++;//ִ����һ��
		}
		else
		{
			Cnt ++;
			if(Cnt>=3)//����3������ִ��
			{
				Cnt=0;
				ConfigModuleNoBlockCaseValue = 0;
			}
		}
		switch(ConfigModuleNoBlockCaseValue)
		{
			case 0://�ر�GPRSģ��
				PDout(10) = 1; CompareValue = 100; DataReturnFlage=1; break;
			case 1://����GPRSģ��
				PDout(10) = 0; CompareValue = 5000; DataReturnFlage=1; break;
			case 2://�ر��ƶ�����,���׹ر�TCP/IP ����
				CompareValue = 3000;//�ָ�3Sһ��ָ��ʱ��
				SendConfigFunction("AT+CIPSHUT\r\n",NULL,"SHUT OK",NULL,FunctionParseCompare,CompareValue);break;
			case 3://��ȡIMEI
				SendConfigFunction("AT+GSN\r\n",NULL,NULL,NULL,FunctionParseGetID,CompareValue);break;
			case 4://��ȡSIM״̬����
				SendConfigFunction("AT+CPIN?\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 5://GPRSע��״̬			
				SendConfigFunction("AT+CGREG?\r\n",NULL,"+CGREG: 0,1",NULL,FunctionParseCompare,CompareValue);break;
			case 6://�Ƿ�����GPRS  
				SendConfigFunction("AT+CGATT?\r\n",NULL,"+CGATT: 1",NULL,FunctionParseCompare,CompareValue);break;			
			case 7://����Ϊ������
				SendConfigFunction("AT+CIPMUX=0\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;			
			case 8://͸��ģʽ 
				SendConfigFunction("AT+CIPMODE=1\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 9://����APN
				SendConfigFunction("AT+CSTT\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 10://�����ƶ�����
				SendConfigFunction("AT+CIICR\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 11://����TCP ������
				MainLen = sprintf(MainBuffer,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",IP,Port);
				MainBuffer[MainLen]=0;
				SendConfigFunction(MainBuffer,NULL,"CONNECT",NULL,FunctionParseCompare,CompareValue);break;
			case 12://����MQTT
				SendConfigFunction(NULL,FunctionParseConnectMqtt,NULL,NULL,FunctionParseConnectMqttAck,CompareValue);break;
			default: 
				SendConfigFunction(NULL,NULL,NULL,NULL,NULL,CompareValue);//�������,������е�ִ�к���
				DataReturnFlage=0; 
				ConfigModuleNoBlockFlage=1; break;
		}
	}
}


/**
* @brief  ��������ָ���
* @param  order          ��������(�ַ���)
* @param  FunctionSend   ��������(���з������ݺ���)
* @param  HopeReturn1    ��������(ϣ�����ص��ַ���1)
* @param  HopeReturn2    ��������(ϣ�����ص��ַ���2)
* @param  FunctionParse  ���ݴ�����
* @param  ConfigFunctionValue  ������һ�����ݵķ���ʱ��,ȡֵΪ ConfigFunctionCntCom:��һ����������
* @retval 
* @example 
**/
void SendConfigFunction(char *order,void (*FunctionSend)(),char *HopeReturn1,char *HopeReturn2,void (*FunctionParse)(char *data),u32 ConfigFunctionValue)
{
	memset(HopeReturnData1,NULL,strlen(HopeReturnData1));
	memset(HopeReturnData2,NULL,strlen(HopeReturnData2));
	if(HopeReturn1!=NULL)  sprintf(HopeReturnData1,"%s",HopeReturn1);//�������ݵ����� HopeReturn1,ϣ�����ص�����1
	if(HopeReturn2!=NULL)  sprintf(HopeReturnData2,"%s",HopeReturn2);//�������ݵ����� HopeReturn1,ϣ�����ص�����2
	if(FunctionSend!=NULL) FunctionSend();//����һ����������ָ��
	
	ConfigConnectDispose = FunctionParse;//���ݴ�����ָ��
	SendNextDelay = ConfigFunctionValue;//�����һ�����ݴ���ɹ�,���Ʒ�����һ�����ݵ�ʱ��
	
	if(order!=NULL){
		MainLen = sprintf((char*)MainBuffer,"%s",order);//���͵�����
		UsartOutStrIT(USART3,MainBuffer,MainLen);
	}
}


/**
* @brief  �������÷�������:�ж��Ƿ񷵻���Ҫ������
* @param  data
* @param  
* @retval 
* @example 
**/
void FunctionParseCompare(char *data)
{
	if(strlen(HopeReturnData1) != 0 && strstr(data, HopeReturnData1))
	{
		DataReturnFlage=1;
		ConfigModuleNoBlockTimerCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockTimerCnt:SendNextDelay);
	}
	if(strlen(HopeReturnData2) != 0 && strstr(data, HopeReturnData2))
	{
		DataReturnFlage=1;
		ConfigModuleNoBlockTimerCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockTimerCnt:SendNextDelay);
	}
}


