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
//u8 coco_publishBuf[PUBLISHBUFMAX];						//发布时候的 数据缓冲区 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


u8 mqtt_connect(void)
{
	u8 coco_mqttConnectName[14];						//存储生成的随机MQTT用户名 寄存器
	u8 connectBuf[CONNECTBUFMAX];
	u16 i;
	int32_t len;
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	memcpy(coco_mqttConnectName,&SIM900_ccid[7],13);
	coco_mqttConnectName[13]=0;
	//############################################  
	data.clientID.cstring =(char*)coco_mqttConnectName;	// 客户端标识符 ID payload
	data.keepAliveInterval = 40;						// keep Alive 单位s
	data.cleansession = 0;								// 清理会话标志置位
	data.username.cstring = "admin";					// 用户名
	data.password.cstring = "password";					// 用户密码
	
	len = MQTTSerialize_connect(connectBuf, sizeof(connectBuf), &data);
	
	#if PNT_ON
	printf("发起连接,%s,%s,%s.\n",data.username.cstring,
			data.password.cstring,data.clientID.cstring);
	#endif
	
	for(i=0;i<len;i++)									// 调用串口发送
		Uart1_sendchar(connectBuf[i]);
	
	for(i=0;i<10000;i++)								// 等待10s返回连接
	{	
		uart1_system();									// U1_rec();	
		if(1==MQTT_login)
			break;
		else
			delay_ms(1);

		IWDG_Feed();									//喂狗
	}
	if(MQTT_login)
		return 1;
	return 0;
}
//------------------------------------------------------------------
//看看字符串有多长 一直检测直到遇到NULL
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
//MQTT发布函数 u8* sp 必须指向字符串 即以'\0'结尾的字符串
//返回1成功 返回0失败
u8 my_mqttPublish(char *sub,char *sp)
{
	u32 payloadlen,len;
	u8 *payload=(u8 *)sp;
	u8 i;
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = sub;
	payloadlen = cocoguojia_strlen((u8 *)sp);	
	len = MQTTSerialize_publish(S1TData, sizeof(S1TData), 0, 1, 0, MQTT_packid++, topicString, (unsigned char*)payload, payloadlen);//这里我改成至少一次了
	
	#if PNT_ON
	printf("发布消息,主题:%s,内容%s.\n",topicString.cstring,payload);
	#endif
	
	for(i=0;i<len;i++)
		Uart1_sendchar(S1TData[i]);

	return 1;
}

//------------------------------------------------
//MQTT订阅函数 u8* sp 必须指向字符串 即以'\0'结尾的字符串
//返回1成功 返回0失败
u8 my_subscribe(char *sp)
{
	int req_qos=1;
	u32 len;
	u32 i;
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = sp;
	len = MQTTSerialize_subscribe(S1TData, sizeof(S1TData), 0, MQTT_packid++, 1, &topicString, &req_qos);//这里我改成至少一次了
	
	#if PNT_ON
	printf("订阅主题:%s.\n",topicString.cstring);
	#endif
	
	for(i=0;i<len;i++)
		Uart1_sendchar(S1TData[i]);

	for(i=0;i<10000;i++)								// 等待10s返回连接
	{	
		uart1_system();									// U1_rec();	
		if((MQTT_suback&0x80)==0x80)
		{
			MQTT_suback&=0x7f;
			return 1;
		}
		else
			delay_ms(1);
		
		IWDG_Feed();									//喂狗
	}
	return 0;
}

//------------------------------------------------
//MQTT处理函数
void MQTT_system()
{
	u8 i;
	
	if(SIM900_sys==0)									//怀疑模组失去响应
	{
		Sim900A_OFF();									//此处硬件关机
		Sim900A_Reast();								//重启模组
	}	
		
	else if(SIM900_sys==1)								//如果TCP断线
	{	
		MQTT_login=0;									//退出登陆
		MQTT_suback=0;									//清空订阅
		Timer3_fg1s=0;									//重置心跳
		Sim900A_Starttcp();								//重新联网
	}	
		
	else if(SIM900_sys==2)								//TCP正常情况下
	{	
		if(!MQTT_login)									//如果MQTT掉线
		{
			for(i=0;i<10;i++)
			{
				if(mqtt_connect())						//连接MQTT服务器
					break;
				if(i>5)									//5次连接不成功应该重启
				{
					SIM900_sys=0;						//报告重启
					break;
				}
			}
		}
		else	
		{	
			LED2=LED_ON;								//开灯
			if((MQTT_suback&0x7f)<MAX_SUB)				//如果没有完全订阅
			{	
				my_subscribe("/ask");					//订阅查询主题
				my_subscribe("/update");				//云升级主题
				my_subscribe("/set");					//设置参数主题
				my_subscribe("/ping");					//测试主题
			}	
				
			if(MQTT_heart_flag)							//发送心跳包
			{
				MQTT_heart_flag=0;
				Uart1_sendchar(0xc0);
				Uart1_sendchar(0x00);
				printf("发送心跳.\n");
			}
		}
	}
}
