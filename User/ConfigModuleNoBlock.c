/**
  ******************************************************************************
  * @file    usart.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2019/10/12
  * @brief   
  ******************************************************************************
	一,使用说明
		1,把以下程序放在1ms定时器中断中
			ConfigModuleNoBlockTimerCnt++;
			
		2,把以下程序放到主函数
		while(1)
		{
			ConfigModuleNoBlock();//配置模块
			
			if(串口接收到一条完整的数据)
			{
			  ConfigConnectDispose(接收的数据地址);
			}
		}
			
  ******************************************************************************
  */
//**********配置模块


#define CONFIGMODULENOBLOCK_C_
#include <stdio.h>

#include "ConfigModuleNoBlock.h"
#include "main.h"
#include "cString.h"
#include "usart.h"
#include "mqtt.h"
#include "System.h"


u32 ConfigModuleNoBlockTimerCnt =0;   //配置函数延时变量,定时器内部累加
int ConfigModuleNoBlockCaseValue = 0; //控制执行哪一条Case 语句
char ConfigModuleNoBlockFlage  = 0;   //1-配置完连接 0-未配置完连接

u32 CompareValue=5000;                //每隔 Ms 发送一次数据
u32 SendNextDelay =0;                 //接收SendConfigFunction函数最后一个参数,最终传递给 ConfigModuleNoBlockCnt 控制写一条数据发送的时间
int Cnt = 0;                          //记录运行状态发送的次数
char DataReturnFlage  = 0;            //是否返回了预期的数据

char HopeReturnData1[20]="";//存储希望返回的数据
char HopeReturnData2[20]="";//存储希望返回的数据

void (*ConfigConnectDispose)(char *data);//定义一个函数指针变量,用来处理模块返回的数据

/*********************************************************************************************************/



/**
* @brief  //获取唯一ID,WIFI获取MAC,GPRS获取IMEI
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
		//如果想使用自定义ID,请屏蔽以上两行
		
	  DataReturnFlage=1;
		ConfigModuleNoBlockTimerCnt = (SendNextDelay == 0 ? ConfigModuleNoBlockTimerCnt:SendNextDelay);
	}
	cStringRestore();
}


/**
* @brief  //组合MQTT协议,并发送AT+CIPSEND=XXX
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
	sprintf(MQTTPublishTopic,"%s%s","device/",&MQTTid[0]);//组合发布的主题
	
	
	mymqtt.mqtt_connect_info.client_id = MQTTid;//client_id
	mymqtt.mqtt_connect_info.keepalive = MQTTkeepAlive;//心跳包时间
	mymqtt.mqtt_connect_info.username = MQTTUserName;//用户名
	mymqtt.mqtt_connect_info.password = MQTTPassWord;///密码
	
	
	mymqtt.mqtt_connect_info.will_topic = MQTTPublishTopic;//遗嘱发布的主题
	mymqtt.mqtt_connect_info.will_message = MQTTWillMessage;//遗嘱的消息
	mymqtt.mqtt_connect_info.will_qos = MQTTWillQos;//遗嘱的消息等级
	mymqtt.mqtt_connect_info.will_retain = MQTTWillRetained;//是否需要服务器保留消息
	mymqtt.mqtt_connect_info.clean_session = 1;//清除连接信息
	
	len = mqtt_connect(&mymqtt,&str);//打包连接信息
	
	if(len>0)
        {
		UsartOutStrIT(USART3,str,len);//发送MQTT协议数据
	}
}


/**
* @brief  //判断是否连接上MQTT
* @param  data:MQTT服务器返回的数据
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
		
		if(mymqtt.connectCb){//调用连接回调函数
			mymqtt.connectCb();
		}
	}
}


/**
* @brief  发送指令配置模块,非阻塞版
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
		if(DataReturnFlage == 1)//上一条指令是OK的
		{
			Cnt=0;
			DataReturnFlage = 0;
			ConfigModuleNoBlockCaseValue ++;//执行下一条
		}
		else
		{
			Cnt ++;
			if(Cnt>=3)//超过3次重新执行
			{
				Cnt=0;
				ConfigModuleNoBlockCaseValue = 0;
			}
		}
		switch(ConfigModuleNoBlockCaseValue)
		{
			case 0://关闭GPRS模块
				PDout(10) = 1; CompareValue = 100; DataReturnFlage=1; break;
			case 1://启动GPRS模块
				PDout(10) = 0; CompareValue = 5000; DataReturnFlage=1; break;
			case 2://关闭移动场景,彻底关闭TCP/IP 连接
				CompareValue = 3000;//恢复3S一次指令时间
				SendConfigFunction("AT+CIPSHUT\r\n",NULL,"SHUT OK",NULL,FunctionParseCompare,CompareValue);break;
			case 3://获取IMEI
				SendConfigFunction("AT+GSN\r\n",NULL,NULL,NULL,FunctionParseGetID,CompareValue);break;
			case 4://读取SIM状态正常
				SendConfigFunction("AT+CPIN?\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 5://GPRS注册状态			
				SendConfigFunction("AT+CGREG?\r\n",NULL,"+CGREG: 0,1",NULL,FunctionParseCompare,CompareValue);break;
			case 6://是否附着了GPRS  
				SendConfigFunction("AT+CGATT?\r\n",NULL,"+CGATT: 1",NULL,FunctionParseCompare,CompareValue);break;			
			case 7://设置为单链接
				SendConfigFunction("AT+CIPMUX=0\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;			
			case 8://透传模式 
				SendConfigFunction("AT+CIPMODE=1\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 9://设置APN
				SendConfigFunction("AT+CSTT\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 10://激活移动场景
				SendConfigFunction("AT+CIICR\r\n",NULL,"OK",NULL,FunctionParseCompare,CompareValue);break;
			case 11://连接TCP 服务器
				MainLen = sprintf(MainBuffer,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",IP,Port);
				MainBuffer[MainLen]=0;
				SendConfigFunction(MainBuffer,NULL,"CONNECT",NULL,FunctionParseCompare,CompareValue);break;
			case 12://连接MQTT
				SendConfigFunction(NULL,FunctionParseConnectMqtt,NULL,NULL,FunctionParseConnectMqttAck,CompareValue);break;
			default: 
				SendConfigFunction(NULL,NULL,NULL,NULL,NULL,CompareValue);//这句必须加,清除所有的执行函数
				DataReturnFlage=0; 
				ConfigModuleNoBlockFlage=1; break;
		}
	}
}


/**
* @brief  发送配置指令函数
* @param  order          发送数据(字符串)
* @param  FunctionSend   发送数据(运行发送数据函数)
* @param  HopeReturn1    接收数据(希望返回的字符串1)
* @param  HopeReturn2    接收数据(希望返回的字符串2)
* @param  FunctionParse  数据处理函数
* @param  ConfigFunctionValue  控制下一条数据的发送时间,取值为 ConfigFunctionCntCom:下一条立即发送
* @retval 
* @example 
**/
void SendConfigFunction(char *order,void (*FunctionSend)(),char *HopeReturn1,char *HopeReturn2,void (*FunctionParse)(char *data),u32 ConfigFunctionValue)
{
	memset(HopeReturnData1,NULL,strlen(HopeReturnData1));
	memset(HopeReturnData2,NULL,strlen(HopeReturnData2));
	if(HopeReturn1!=NULL)  sprintf(HopeReturnData1,"%s",HopeReturn1);//拷贝数据到数组 HopeReturn1,希望返回的数据1
	if(HopeReturn2!=NULL)  sprintf(HopeReturnData2,"%s",HopeReturn2);//拷贝数据到数组 HopeReturn1,希望返回的数据2
	if(FunctionSend!=NULL) FunctionSend();//调用一个函数发送指令
	
	ConfigConnectDispose = FunctionParse;//传递处理函数指针
	SendNextDelay = ConfigFunctionValue;//如果上一条数据处理成功,控制发送下一条数据的时间
	
	if(order!=NULL){
		MainLen = sprintf((char*)MainBuffer,"%s",order);//发送的数据
		UsartOutStrIT(USART3,MainBuffer,MainLen);
	}
}


/**
* @brief  处理配置返回数据:判断是否返回想要的数据
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


