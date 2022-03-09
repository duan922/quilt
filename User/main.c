
#define __MAIN_C_

#include "main.h"
#include <stdio.h>
#include "mqtt.h"
#include "usart.h"
#include "gpio.h"
#include "System.h"
#include "delay.h"
#include "iwdg.h"
#include "timer.h"
#include "DHT11.h"
#include "ConfigModuleNoBlock.h"

#include "myiic.h"
#include "SHT20.h"
#include "lock.h"
#include "max31865.h"
#include "usart2.h"
#include "adc.h"
#include <math.h>
#include "mpu6050.h"
#include "mpuiic.h"
#include "uart5.h"
#include "exti.h"
#include "wk2xxx.h"

//缓存烟雾数据
u8 table1[20];   
u8 table2[20];
u8 table3[20];
//缓存数据使用
unsigned char MainBuffer[MainBufferLen];//缓存数据,全局通用
u32  MainLen=0;      //全局通用变量
unsigned char *MainString;    //全局通用变量

char RelayState = 0;//记录继电器状态
char HartState = 0;//记录继电器状态
char LockState = 0;//记录继电器状态
char QuiltState = 0;//记录继电器状态
char PosState = 0;//记录继电器状态


extern unsigned short int length_val;
extern unsigned char rxbuf[16],rxempty, rxcnt,rxflag,waitflag;
extern unsigned int timeout;


//连接MQTT
unsigned char IP[55]="116.62.44.118";//IP地址/域名
unsigned char Port[7] = "1883";//端口号
unsigned char MQTTid[50] = "";//ClientID
unsigned char MQTTUserName[20] = "admin";//用户名
unsigned char MQTTPassWord[50] = "public";//密码
unsigned char MQTTkeepAlive = 20;//心跳包时间

unsigned char MQTTPublishTopic[30]="";//存储MQTT发布的主题
unsigned char MQTTWillMessage[50] = "{\"data\":\"status\",\"status\":\"offline\"}";//遗嘱消息
unsigned char MQTTWillQos = 0;//消息等级
unsigned char MQTTWillRetained = 1;//是否需要服务器记录
unsigned char MQTTSubscribeTopic[30]="";//存储MQTT订阅的主题

unsigned char MY1PublishTopic[30]="bind";//存储MQTT发布的主题////我定义
unsigned char MY2PublishTopic[30]="listen_1";//存储MQTT发布的主题////我定义
unsigned char MY1SubscribeTopic[30]="open";//存储MQTT订阅的主题///我的
unsigned char MY2SubscribeTopic[30]="listen_2";//存储MQTT订阅的主题///我的
u8 PZEM[8]={0x01,0x04,0x00,0x00,0x00,0x0A,0x70,0x0D};

void RelayFunction(void);//继电器状态检测
void TEMP_Function(void);//SHT20,温湿度采集
void AC_Function(void);

int HartFunction(void);//加热状态
int LockFunction(void);//门锁状态
int LaserFunction(void);//被子状态检测
//int MQ_2Function(void);//烟雾报警
int posture_Function(void);

void MqttConnect(void);//连接上MQTT回调
void MqttDisConnect(void);//MQTT断开连接回调
void subscribedCb(int pdata);//订阅成功回调
void failsubscribedCb(int pdata);//订阅失败回调
void PublishedCb(void);//成功发布消息回调
void MqttReceive(const char* topic, uint32_t topic_len,const char *data, uint32_t lengh);//接收到数据回调

int main(void)
{
    
    NVIC_PriorityGroup();
		uart_init(115200,115200);	 //串口1、3初始化为115200
    Usart2_Init(9600);           //串口2初始化为9600，激光
		GpioInit();

		DelayInit();
		Timer2_Config();
//    TIM3_Int_Init(65535,65535);//定时1分钟
    TIM3_Int_Init(30000,7199);//定时3s
    
    IIC_Init();
    SHT2x_Init();
    LOCK_Init();
    MAX31865_Init();  //温度
    MAX31865_Cfg();  //
    Adc_Init();      //烟雾
    MPU_Init();
    uart5_init(115200);	//串口5初始化
    WK_RstInit();      //复位初始化
    Wk_BaudAdaptive();  //函数功能:主串口波特率匹配 
    //初始化串口。
    Wk_Init(1);
		Wk_Init(2);
		Wk_Init(3);
		Wk_Init(4);
    
    Wk_SetBaud(1,B9600);
		Wk_SetBaud(2,B115200);
		Wk_SetBaud(3,B115200);
		Wk_SetBaud(4,B115200);
	EXTIX_Init();//初始化CPU外部中断
	/*MQTT*/
	mqtt_init(&mymqtt);
	mqtt_connect_reg(&mymqtt,MqttConnect);//注册连接回调函数
	mqtt_disconnect_reg(&mymqtt,MqttDisConnect);//注册断开连接回调函数
	mqtt_received_reg(&mymqtt,MqttReceive);//注册接收数据回调函数

	IWDG_Init(IWDG_Prescaler_256,156*5);
	
	printf("start\r\n");
	
	
	while(1)
	{
		IWDG_Feed();//喂狗
		LockState= LockFunction();//检测锁
		QuiltState= LaserFunction();//检测被子
		PosState= posture_Function();//检测姿态
//		LED_light=0;
//		delay_ms(1000);
		if((LOCK==0)&&(LED_yellow==0))
		{		
			light_color(2);							//空闲亮     绿灯
		}
		 ConfigModuleNoBlock();
		if(ConfigModuleNoBlockFlage)//(模块连接了MQTT)
		{
			mqtt_send_function(&mymqtt);//提取发送缓存的MQTT协议
			mqtt_keep_alive(&mymqtt);//处理发送心跳包           
		}				
		//读取串口接收的数据
		BufferManageRead(&buff_manage_struct_usart3_read,Usart3BufferMemoryCopy,&buff_manage_struct_usart3_read.ReadLen);
        
		if(buff_manage_struct_usart3_read.ReadLen>0)//模块返回了数据
		{
			if(ConfigConnectDispose != NULL)
			{
				ConfigConnectDispose((char*)Usart3BufferMemoryCopy);//处理模块返回的数据
				memset(Usart3BufferMemoryCopy,0,buff_manage_struct_usart3_read.ReadLen);
			}			
			if(ConfigModuleNoBlockFlage)//(模块连接了MQTT)
			{
				mqtt_read_function(&mymqtt,Usart3BufferMemoryCopy,buff_manage_struct_usart3_read.ReadLen);//函数处理MQTT数据
			}
		} 
//		RelayFunction();		
		TEMP_Function();
     
	}
}



/**
* @brief   继电器状态检测
**/
void RelayFunction(void)
{
	  u8  fan_status,hart_status,lock_status,quilt_status,Pos_status;

    fan_status=fan;   //高-空闲状态 
	  hart_status=heater;//加热
	  lock_status=LockState;//锁
    quilt_status=QuiltState;//激光
		Pos_status=PosState;//姿态
    if(ConfigModuleNoBlockFlage)//配置模块成功(模块连接了MQTT)
		{             
			MainLen = sprintf((char*)MainBuffer,"{\"ID\":\"\%s\",\"fan\":\"%d\",\"hart\":\"%d\",\"quilt\":\"%d\",\"lock\":\"%d\",\"pos\":\"%d\"}",&MQTTid[0],fan_status,hart_status,quilt_status,lock_status,Pos_status);//是否开启
      mqtt_publish(&mymqtt,MY1PublishTopic,MainBuffer,MainLen, 0, 1,PublishedCb);//第一次发布消息bind  
			delay_ms(1000);                
    }
}

/**
* @brief   门锁状态检测
**/
int LockFunction(void)
{
	char Lock_Fun;
   if((LOCK_s1||LOCK_s3)==0)
   {
        Lock_Fun=0;//初始
//				LED_light=1;//开门就亮灯
   }
   else if((LOCK_s1&&LOCK_s2==1))
   {
        Lock_Fun=1;//电磁合上
   }
   else if((LOCK_s1&&LOCK_s3==1))
   {
        Lock_Fun=2;//开锁
   }
	 else 
	 {
				if(LOCK&&LOCK_s3==0)
				{
						Lock_Fun=3;//故障
						light_color(1);
				}
	 }
   return Lock_Fun;
}


/**
* @brief  激光状态检测
**/
int LaserFunction(void)
{
   unsigned char i,quilt_value;	
	if(rxflag)//接收到1组有效数据
		{
			for(i=0; i<rxcnt; i++)
			{
				if(rxbuf[i]=='m')
				{
					if(rxbuf[i+1]=='m')	//ASCII码转换为16进制数据，单位mm
					{
                       
						if((i>0)&&(rxbuf[i-1]>='0')&&(rxbuf[i-1]<='9'))
							length_val=rxbuf[i-1]-'0';
						if((i>1)&&(rxbuf[i-2]>='0')&&(rxbuf[i-2]<='9'))
							length_val+=(rxbuf[i-2]-'0')*10;
						if((i>2)&&(rxbuf[i-3]>='0')&&(rxbuf[i-3]<='9'))
							length_val+=(rxbuf[i-3]-'0')*100;
						if((i>3)&&(rxbuf[i-4]>='0')&&(rxbuf[i-4]<='9'))
							length_val+=(rxbuf[i-4]-'0')*1000;                     
						break;
					}
				}
			}       
			rxflag = 0;
			rxcnt = 0;
		}
    if(length_val<=500)quilt_value=1;//有被子
        else quilt_value=0;          //无被子
        return quilt_value;
}	

/**
* @brief  烟雾报警检测
**/
//int MQ_2Function(void)
//{
//    u16 adcx;
//	float temp;
//	float value=2.5;
//	float ppm;
//    adcx=Get_Adc_Average(ADC_Channel_1,10);//采集电压值
//    sprintf((char*)table1,"%d",adcx);
//    temp=(float)adcx*(3.3/4096);
//	ppm=pow((0.1/temp)-0.02,-1.52695067);
//	sprintf((char*)table3,"%f",ppm);
//	sprintf((char*)table2,"%f",temp);
//  return 0;
//}

/**
* @brief  电流检测
**/
void AC_Function(void)
{
    if(ConfigModuleNoBlockFlage)//配置模块成功(模块连接了MQTT)
    {
     Exti_Disable();/*关闭外部中断响应*/
		 wk_TxChars(1,8,PZEM);/*写入len个数据进入发送fifo*/
     delay_ms(500);	
     Exti_Enable();/*开启外部中断响应*/             
    }            
}
/**
* @brief   姿态采集
**/
int posture_Function(void)
{
		float status_x,status_y;
		short aacx,aacy,aacz;		//加速度传感器原始数据
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据\"X\":\"%2.1f\",\"Y\":\"%3.1f°\",,status_x,status_y
    status_x=aacx/200.0;
    status_y=aacy/200.0;
	if((fabs(status_x)>45.0)||(fabs(status_y)>45.0))
	{
//		light_color(1);//红灯
		PosState=1;
//		delay_ms(100);
//		LED_red=0;
	}else 
	{
		PosState=0;
	}
	return (PosState);
}

/**
* @brief   SHT20+PT100温湿度采集
**/
void TEMP_Function(void)
{  
		float sht_temp,sht_hum,max_temp1,max_temp2;      
    AC_Function();
		if(RendTHCnt>=2000)
	{    
        sht_temp=SHT2x_GetTempPoll();
				sht_hum =SHT2x_GetHumiPoll();
        max_temp1=MAX31865_GetTemp(0);
        max_temp2=MAX31865_GetTemp(1);

        RendTHCnt=0;
		if(ConfigModuleNoBlockFlage)//配置模块成功(模块连接了MQTT)
		{
				MainLen = sprintf((char*)MainBuffer,"{\"ID\":\"%s\",\"temp1\":\"%2.1f\",\"hum\":\"%2.1f\",\"temp2\":\"%2.1f\",\"temp3\":\"%2.1f\",\"voltage\":\"%3.1fV\",\"current\":\"%3.3fA\"}\r\n",&MQTTid[0],sht_temp,sht_hum,max_temp1,max_temp2,AC_Voltage,AC_Current);			
        mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息  ******************************* 发布主题为   device/id                
		}
	}
}


// @brief   连接上MQTT回调函数

void MqttConnect()
{
		memset(MQTTSubscribeTopic,NULL,sizeof(MQTTSubscribeTopic));
		//sprintf((char*)MQTTSubscribeTopic,"%s%s","user/",&MQTTid[0]);//组合字符串
		mqtt_subscribe(&mymqtt,MY1SubscribeTopic,0,subscribedCb,failsubscribedCb);//订阅主题1
   	mqtt_subscribe(&mymqtt,MY2SubscribeTopic,0,subscribedCb,failsubscribedCb);//订阅主题2
 
}


//@brief   MQTT断开连接回调
void MqttDisConnect()
{
	mqtt_init(&mymqtt);	
	//重新配置模块连接MQTT
	ConfigModuleNoBlockCaseValue=0;
	ConfigModuleNoBlockFlage = 0;
	
	printf("\r\n连接断开********************************\r\n");
}


/**
* @brief   MQTT接收数据回调
* @param   topic:主题
* @param   topic_len:主题长度
* @param   data:接收的数据
* @param   lengh:接收的数据长度
**/
void MqttReceive(const char* topic, uint32_t topic_len,const char *data, uint32_t lengh)
{
   	sprintf((char*)MainBuffer,"\"id\":\"%s\"",&MQTTid[0]);//发送的数据
    if(strstr((char*)data,MainBuffer))//询问id868956047824309
       {
           if(strstr((char*)data,"\"time\":20")) //询问时间                                 {"id":"868956047822907","time":20}
           {            
             xuan=1;
						 light_color(1);		//红灯
							LOCK=1;
              fan=0;            //开启风扇
//              UV_lamp=0;        //开启紫外灯
              TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源
              TIM_SetCounter(TIM3,0); //计数器清空                    
              TIM_Cmd(TIM3, ENABLE);  //使能TIM3         			
							MainLen = sprintf((char*)MainBuffer,"{\"mode\":\"%d\"}",1);//发送的数据
							mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息  ******************************* 发布主题为   listen1                
           }
           
           else if(strstr((char*)data,"\"time\":30"))
           {           
             xuan=2;
							light_color(2);		//绿灯
							LOCK=1;
              fan=0;            //开启风扇
              UV_lamp=0;        //开启紫外灯
              TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  //清除TIMx的中断待处理位:TIM 中断源
              TIM_SetCounter(TIM3,0); //计数器清空                    
              TIM_Cmd(TIM3, ENABLE);  //使能TIM3  
        			MainLen = sprintf((char*)MainBuffer,"{\"mode\":\"%d\"}",2);//发送的数据
							mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息  ******************************* 发布主题为   listen1               
           }
           
           else if(strstr((char*)data,"\"time\":40"))
           {           
              xuan=3;
						 light_color(2);		//绿灯
							LOCK=1;
              fan=0;            //开启风扇
              UV_lamp=0;        //开启紫外灯
              TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源
              TIM_SetCounter(TIM3,0); //计数器清空                    
              TIM_Cmd(TIM3, ENABLE);  //使能TIM3        			
							MainLen = sprintf((char*)MainBuffer,"{\"mode\":\"%d\"}",3);//发送的数据
							mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息  ******************************* 发布主题为   listen1                 
           }
           
           else if(strstr((char*)data,"\"time\":50"))
           {           
              xuan=4;
							light_color(2);		//绿灯
							LOCK=1;
              fan=0;            //开启风扇
              UV_lamp=0;        //开启紫外灯
              TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源
              TIM_SetCounter(TIM3,0); //计数器清空                    
              TIM_Cmd(TIM3, ENABLE);  //使能TIM3         			
							MainLen = sprintf((char*)MainBuffer,"{\"mode\":\"%d\"}",4);//发送的数据
							mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息  ******************************* 发布主题为   listen1          
           }
           
       }
       if(strstr((char*)data,"\"status\":0"))//关机
       {
           LOCK=0;
				   light_color(0);		//关灯
           heater=1;			//关
           UV_lamp=1;			//关
           MainLen = sprintf((char*)MainBuffer,"{\"status\":\"%s\"}","close");//发送的数据
           mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//发布消息  ******************************* 发布主题为  listen1 
           
       }
          
}


/**
* @brief   订阅主题成功

**/
void subscribedCb(int pdata)
{
	printf("\r\n成功订阅主题\r\n");
}

/**
* @brief   订阅主题失败
**/
void failsubscribedCb(int pdata)
{
	printf("\r\n订阅主题失败\r\n");
}


/**
* @brief   发布成功
**/
void PublishedCb()
{
	printf("\r\n发布成功\r\n");
}



