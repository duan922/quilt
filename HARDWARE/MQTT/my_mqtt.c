#include "sys.h"
//#include "gpio.h"
#include "delay.h"
#include "timer.h" 
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "eeprom.h"
#include "system.h"
#include "string.h"
#include "sim900a.h"
#include "my_mqtt.h"
#include "MQTTPacket.h"


#define CONNECTBUFMAX 70
#define PUBLISHBUFMAX 300
//u8 coco_publishBuf[PUBLISHBUFMAX];						//����ʱ��� ���ݻ����� 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


u8 mqtt_connect(void)
{
	u8 coco_mqttConnectName[14];						//�洢���ɵ����MQTT�û��� �Ĵ���
	u8 connectBuf[CONNECTBUFMAX];
	u16 i;
	int32_t len;
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	memcpy(coco_mqttConnectName,&SIM900_ccid[7],13);
	coco_mqttConnectName[13]=0;
	//############################################  
	data.clientID.cstring =(char*)coco_mqttConnectName;	// �ͻ��˱�ʶ�� ID payload
	data.keepAliveInterval = 40;						// keep Alive ��λs
	data.cleansession = 0;								// ����Ự��־��λ
	data.username.cstring = "admin";					// �û���
	data.password.cstring = "password";					// �û�����
	
	len = MQTTSerialize_connect(connectBuf, sizeof(connectBuf), &data);
	
	#if PNT_ON
	printf("��������,%s,%s,%s.\n",data.username.cstring,
			data.password.cstring,data.clientID.cstring);
	#endif
	
	for(i=0;i<len;i++)									// ���ô��ڷ���
		Uart1_sendchar(connectBuf[i]);
	
	for(i=0;i<10000;i++)								// �ȴ�10s��������
	{	
		uart1_system();									// U1_rec();	
		if(1==MQTT_login)
			break;
		else
			delay_ms(1);

		IWDG_Feed();									//ι��
	}
	if(MQTT_login)
		return 1;
	return 0;
}
//------------------------------------------------------------------
//�����ַ����ж೤ һֱ���ֱ������NULL
static u16 cocoguojia_strlen(u8 *sp)
{
	u16 n=0;
	u8 *tr=sp;
	while(0!=(*tr))
	{
		tr++;
		n++;
	}
	return n;
}


//------------------------------------------------
//MQTT�������� u8* sp ����ָ���ַ��� ����'\0'��β���ַ���
//����1�ɹ� ����0ʧ��
u8 my_mqttPublish(char *sub,char *sp)
{
	u32 payloadlen,len;
	u8 *payload=(u8 *)sp;
	u8 i;
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = sub;
	payloadlen = cocoguojia_strlen((u8 *)sp);	
	len = MQTTSerialize_publish(S1TData, sizeof(S1TData), 0, 1, 0, MQTT_packid++, topicString, (unsigned char*)payload, payloadlen);//�����Ҹĳ�����һ����
	
	#if PNT_ON
	printf("������Ϣ,����:%s,����%s.\n",topicString.cstring,payload);
	#endif
	
	for(i=0;i<len;i++)
		Uart1_sendchar(S1TData[i]);

	return 1;
}

//------------------------------------------------
//MQTT���ĺ��� u8* sp ����ָ���ַ��� ����'\0'��β���ַ���
//����1�ɹ� ����0ʧ��
u8 my_subscribe(char *sp)
{
	int req_qos=1;
	u32 len;
	u32 i;
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = sp;
	len = MQTTSerialize_subscribe(S1TData, sizeof(S1TData), 0, MQTT_packid++, 1, &topicString, &req_qos);//�����Ҹĳ�����һ����
	
	#if PNT_ON
	printf("��������:%s.\n",topicString.cstring);
	#endif
	
	for(i=0;i<len;i++)
		Uart1_sendchar(S1TData[i]);

	for(i=0;i<10000;i++)								// �ȴ�10s��������
	{	
		uart1_system();									// U1_rec();	
		if((MQTT_suback&0x80)==0x80)
		{
			MQTT_suback&=0x7f;
			return 1;
		}
		else
			delay_ms(1);
		
		IWDG_Feed();									//ι��
	}
	return 0;
}

//------------------------------------------------
//MQTT������
void MQTT_system()
{
	u8 i;
	
	if(SIM900_sys==0)									//����ģ��ʧȥ��Ӧ
	{
		Sim900A_OFF();									//�˴�Ӳ���ػ�
		Sim900A_Reast();								//����ģ��
	}	
		
	else if(SIM900_sys==1)								//���TCP����
	{	
		MQTT_login=0;									//�˳���½
		MQTT_suback=0;									//��ն���
		Timer3_fg1s=0;									//��������
		Sim900A_Starttcp();								//��������
	}	
		
	else if(SIM900_sys==2)								//TCP���������
	{	
		if(!MQTT_login)									//���MQTT����
		{
			for(i=0;i<10;i++)
			{
				if(mqtt_connect())						//����MQTT������
					break;
				if(i>5)									//5�����Ӳ��ɹ�Ӧ������
				{
					SIM900_sys=0;						//��������
					break;
				}
			}
		}
		else	
		{	
			LED2=LED_ON;								//����
			if((MQTT_suback&0x7f)<MAX_SUB)				//���û����ȫ����
			{	
				my_subscribe("/ask");					//���Ĳ�ѯ����
				my_subscribe("/update");				//����������
				my_subscribe("/set");					//���ò�������
				my_subscribe("/ping");					//��������
			}	
				
			if(MQTT_heart_flag)							//����������
			{
				MQTT_heart_flag=0;
				Uart1_sendchar(0xc0);
				Uart1_sendchar(0x00);
				printf("��������.\n");
			}
		}
	}
}
