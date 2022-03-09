
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

//������������
u8 table1[20];   
u8 table2[20];
u8 table3[20];
//��������ʹ��
unsigned char MainBuffer[MainBufferLen];//��������,ȫ��ͨ��
u32  MainLen=0;      //ȫ��ͨ�ñ���
unsigned char *MainString;    //ȫ��ͨ�ñ���

char RelayState = 0;//��¼�̵���״̬
char HartState = 0;//��¼�̵���״̬
char LockState = 0;//��¼�̵���״̬
char QuiltState = 0;//��¼�̵���״̬
char PosState = 0;//��¼�̵���״̬


extern unsigned short int length_val;
extern unsigned char rxbuf[16],rxempty, rxcnt,rxflag,waitflag;
extern unsigned int timeout;


//����MQTT
unsigned char IP[55]="116.62.44.118";//IP��ַ/����
unsigned char Port[7] = "1883";//�˿ں�
unsigned char MQTTid[50] = "";//ClientID
unsigned char MQTTUserName[20] = "admin";//�û���
unsigned char MQTTPassWord[50] = "public";//����
unsigned char MQTTkeepAlive = 20;//������ʱ��

unsigned char MQTTPublishTopic[30]="";//�洢MQTT����������
unsigned char MQTTWillMessage[50] = "{\"data\":\"status\",\"status\":\"offline\"}";//������Ϣ
unsigned char MQTTWillQos = 0;//��Ϣ�ȼ�
unsigned char MQTTWillRetained = 1;//�Ƿ���Ҫ��������¼
unsigned char MQTTSubscribeTopic[30]="";//�洢MQTT���ĵ�����

unsigned char MY1PublishTopic[30]="bind";//�洢MQTT����������////�Ҷ���
unsigned char MY2PublishTopic[30]="listen_1";//�洢MQTT����������////�Ҷ���
unsigned char MY1SubscribeTopic[30]="open";//�洢MQTT���ĵ�����///�ҵ�
unsigned char MY2SubscribeTopic[30]="listen_2";//�洢MQTT���ĵ�����///�ҵ�
u8 PZEM[8]={0x01,0x04,0x00,0x00,0x00,0x0A,0x70,0x0D};

void RelayFunction(void);//�̵���״̬���
void TEMP_Function(void);//SHT20,��ʪ�Ȳɼ�
void AC_Function(void);

int HartFunction(void);//����״̬
int LockFunction(void);//����״̬
int LaserFunction(void);//����״̬���
//int MQ_2Function(void);//������
int posture_Function(void);

void MqttConnect(void);//������MQTT�ص�
void MqttDisConnect(void);//MQTT�Ͽ����ӻص�
void subscribedCb(int pdata);//���ĳɹ��ص�
void failsubscribedCb(int pdata);//����ʧ�ܻص�
void PublishedCb(void);//�ɹ�������Ϣ�ص�
void MqttReceive(const char* topic, uint32_t topic_len,const char *data, uint32_t lengh);//���յ����ݻص�

int main(void)
{
    
    NVIC_PriorityGroup();
		uart_init(115200,115200);	 //����1��3��ʼ��Ϊ115200
    Usart2_Init(9600);           //����2��ʼ��Ϊ9600������
		GpioInit();

		DelayInit();
		Timer2_Config();
//    TIM3_Int_Init(65535,65535);//��ʱ1����
    TIM3_Int_Init(30000,7199);//��ʱ3s
    
    IIC_Init();
    SHT2x_Init();
    LOCK_Init();
    MAX31865_Init();  //�¶�
    MAX31865_Cfg();  //
    Adc_Init();      //����
    MPU_Init();
    uart5_init(115200);	//����5��ʼ��
    WK_RstInit();      //��λ��ʼ��
    Wk_BaudAdaptive();  //��������:�����ڲ�����ƥ�� 
    //��ʼ�����ڡ�
    Wk_Init(1);
		Wk_Init(2);
		Wk_Init(3);
		Wk_Init(4);
    
    Wk_SetBaud(1,B9600);
		Wk_SetBaud(2,B115200);
		Wk_SetBaud(3,B115200);
		Wk_SetBaud(4,B115200);
	EXTIX_Init();//��ʼ��CPU�ⲿ�ж�
	/*MQTT*/
	mqtt_init(&mymqtt);
	mqtt_connect_reg(&mymqtt,MqttConnect);//ע�����ӻص�����
	mqtt_disconnect_reg(&mymqtt,MqttDisConnect);//ע��Ͽ����ӻص�����
	mqtt_received_reg(&mymqtt,MqttReceive);//ע��������ݻص�����

	IWDG_Init(IWDG_Prescaler_256,156*5);
	
	printf("start\r\n");
	
	
	while(1)
	{
		IWDG_Feed();//ι��
		LockState= LockFunction();//�����
		QuiltState= LaserFunction();//��ⱻ��
		PosState= posture_Function();//�����̬
//		LED_light=0;
//		delay_ms(1000);
		if((LOCK==0)&&(LED_yellow==0))
		{		
			light_color(2);							//������     �̵�
		}
		 ConfigModuleNoBlock();
		if(ConfigModuleNoBlockFlage)//(ģ��������MQTT)
		{
			mqtt_send_function(&mymqtt);//��ȡ���ͻ����MQTTЭ��
			mqtt_keep_alive(&mymqtt);//������������           
		}				
		//��ȡ���ڽ��յ�����
		BufferManageRead(&buff_manage_struct_usart3_read,Usart3BufferMemoryCopy,&buff_manage_struct_usart3_read.ReadLen);
        
		if(buff_manage_struct_usart3_read.ReadLen>0)//ģ�鷵��������
		{
			if(ConfigConnectDispose != NULL)
			{
				ConfigConnectDispose((char*)Usart3BufferMemoryCopy);//����ģ�鷵�ص�����
				memset(Usart3BufferMemoryCopy,0,buff_manage_struct_usart3_read.ReadLen);
			}			
			if(ConfigModuleNoBlockFlage)//(ģ��������MQTT)
			{
				mqtt_read_function(&mymqtt,Usart3BufferMemoryCopy,buff_manage_struct_usart3_read.ReadLen);//��������MQTT����
			}
		} 
//		RelayFunction();		
		TEMP_Function();
     
	}
}



/**
* @brief   �̵���״̬���
**/
void RelayFunction(void)
{
	  u8  fan_status,hart_status,lock_status,quilt_status,Pos_status;

    fan_status=fan;   //��-����״̬ 
	  hart_status=heater;//����
	  lock_status=LockState;//��
    quilt_status=QuiltState;//����
		Pos_status=PosState;//��̬
    if(ConfigModuleNoBlockFlage)//����ģ��ɹ�(ģ��������MQTT)
		{             
			MainLen = sprintf((char*)MainBuffer,"{\"ID\":\"\%s\",\"fan\":\"%d\",\"hart\":\"%d\",\"quilt\":\"%d\",\"lock\":\"%d\",\"pos\":\"%d\"}",&MQTTid[0],fan_status,hart_status,quilt_status,lock_status,Pos_status);//�Ƿ���
      mqtt_publish(&mymqtt,MY1PublishTopic,MainBuffer,MainLen, 0, 1,PublishedCb);//��һ�η�����Ϣbind  
			delay_ms(1000);                
    }
}

/**
* @brief   ����״̬���
**/
int LockFunction(void)
{
	char Lock_Fun;
   if((LOCK_s1||LOCK_s3)==0)
   {
        Lock_Fun=0;//��ʼ
//				LED_light=1;//���ž�����
   }
   else if((LOCK_s1&&LOCK_s2==1))
   {
        Lock_Fun=1;//��ź���
   }
   else if((LOCK_s1&&LOCK_s3==1))
   {
        Lock_Fun=2;//����
   }
	 else 
	 {
				if(LOCK&&LOCK_s3==0)
				{
						Lock_Fun=3;//����
						light_color(1);
				}
	 }
   return Lock_Fun;
}


/**
* @brief  ����״̬���
**/
int LaserFunction(void)
{
   unsigned char i,quilt_value;	
	if(rxflag)//���յ�1����Ч����
		{
			for(i=0; i<rxcnt; i++)
			{
				if(rxbuf[i]=='m')
				{
					if(rxbuf[i+1]=='m')	//ASCII��ת��Ϊ16�������ݣ���λmm
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
    if(length_val<=500)quilt_value=1;//�б���
        else quilt_value=0;          //�ޱ���
        return quilt_value;
}	

/**
* @brief  ���������
**/
//int MQ_2Function(void)
//{
//    u16 adcx;
//	float temp;
//	float value=2.5;
//	float ppm;
//    adcx=Get_Adc_Average(ADC_Channel_1,10);//�ɼ���ѹֵ
//    sprintf((char*)table1,"%d",adcx);
//    temp=(float)adcx*(3.3/4096);
//	ppm=pow((0.1/temp)-0.02,-1.52695067);
//	sprintf((char*)table3,"%f",ppm);
//	sprintf((char*)table2,"%f",temp);
//  return 0;
//}

/**
* @brief  �������
**/
void AC_Function(void)
{
    if(ConfigModuleNoBlockFlage)//����ģ��ɹ�(ģ��������MQTT)
    {
     Exti_Disable();/*�ر��ⲿ�ж���Ӧ*/
		 wk_TxChars(1,8,PZEM);/*д��len�����ݽ��뷢��fifo*/
     delay_ms(500);	
     Exti_Enable();/*�����ⲿ�ж���Ӧ*/             
    }            
}
/**
* @brief   ��̬�ɼ�
**/
int posture_Function(void)
{
		float status_x,status_y;
		short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������\"X\":\"%2.1f\",\"Y\":\"%3.1f��\",,status_x,status_y
    status_x=aacx/200.0;
    status_y=aacy/200.0;
	if((fabs(status_x)>45.0)||(fabs(status_y)>45.0))
	{
//		light_color(1);//���
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
* @brief   SHT20+PT100��ʪ�Ȳɼ�
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
		if(ConfigModuleNoBlockFlage)//����ģ��ɹ�(ģ��������MQTT)
		{
				MainLen = sprintf((char*)MainBuffer,"{\"ID\":\"%s\",\"temp1\":\"%2.1f\",\"hum\":\"%2.1f\",\"temp2\":\"%2.1f\",\"temp3\":\"%2.1f\",\"voltage\":\"%3.1fV\",\"current\":\"%3.3fA\"}\r\n",&MQTTid[0],sht_temp,sht_hum,max_temp1,max_temp2,AC_Voltage,AC_Current);			
        mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ  ******************************* ��������Ϊ   device/id                
		}
	}
}


// @brief   ������MQTT�ص�����

void MqttConnect()
{
		memset(MQTTSubscribeTopic,NULL,sizeof(MQTTSubscribeTopic));
		//sprintf((char*)MQTTSubscribeTopic,"%s%s","user/",&MQTTid[0]);//����ַ���
		mqtt_subscribe(&mymqtt,MY1SubscribeTopic,0,subscribedCb,failsubscribedCb);//��������1
   	mqtt_subscribe(&mymqtt,MY2SubscribeTopic,0,subscribedCb,failsubscribedCb);//��������2
 
}


//@brief   MQTT�Ͽ����ӻص�
void MqttDisConnect()
{
	mqtt_init(&mymqtt);	
	//��������ģ������MQTT
	ConfigModuleNoBlockCaseValue=0;
	ConfigModuleNoBlockFlage = 0;
	
	printf("\r\n���ӶϿ�********************************\r\n");
}


/**
* @brief   MQTT�������ݻص�
* @param   topic:����
* @param   topic_len:���ⳤ��
* @param   data:���յ�����
* @param   lengh:���յ����ݳ���
**/
void MqttReceive(const char* topic, uint32_t topic_len,const char *data, uint32_t lengh)
{
   	sprintf((char*)MainBuffer,"\"id\":\"%s\"",&MQTTid[0]);//���͵�����
    if(strstr((char*)data,MainBuffer))//ѯ��id868956047824309
       {
           if(strstr((char*)data,"\"time\":20")) //ѯ��ʱ��                                 {"id":"868956047822907","time":20}
           {            
             xuan=1;
						 light_color(1);		//���
							LOCK=1;
              fan=0;            //��������
//              UV_lamp=0;        //���������
              TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ
              TIM_SetCounter(TIM3,0); //���������                    
              TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3         			
							MainLen = sprintf((char*)MainBuffer,"{\"mode\":\"%d\"}",1);//���͵�����
							mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ  ******************************* ��������Ϊ   listen1                
           }
           
           else if(strstr((char*)data,"\"time\":30"))
           {           
             xuan=2;
							light_color(2);		//�̵�
							LOCK=1;
              fan=0;            //��������
              UV_lamp=0;        //���������
              TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  //���TIMx���жϴ�����λ:TIM �ж�Դ
              TIM_SetCounter(TIM3,0); //���������                    
              TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3  
        			MainLen = sprintf((char*)MainBuffer,"{\"mode\":\"%d\"}",2);//���͵�����
							mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ  ******************************* ��������Ϊ   listen1               
           }
           
           else if(strstr((char*)data,"\"time\":40"))
           {           
              xuan=3;
						 light_color(2);		//�̵�
							LOCK=1;
              fan=0;            //��������
              UV_lamp=0;        //���������
              TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ
              TIM_SetCounter(TIM3,0); //���������                    
              TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3        			
							MainLen = sprintf((char*)MainBuffer,"{\"mode\":\"%d\"}",3);//���͵�����
							mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ  ******************************* ��������Ϊ   listen1                 
           }
           
           else if(strstr((char*)data,"\"time\":50"))
           {           
              xuan=4;
							light_color(2);		//�̵�
							LOCK=1;
              fan=0;            //��������
              UV_lamp=0;        //���������
              TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ
              TIM_SetCounter(TIM3,0); //���������                    
              TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3         			
							MainLen = sprintf((char*)MainBuffer,"{\"mode\":\"%d\"}",4);//���͵�����
							mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ  ******************************* ��������Ϊ   listen1          
           }
           
       }
       if(strstr((char*)data,"\"status\":0"))//�ػ�
       {
           LOCK=0;
				   light_color(0);		//�ص�
           heater=1;			//��
           UV_lamp=1;			//��
           MainLen = sprintf((char*)MainBuffer,"{\"status\":\"%s\"}","close");//���͵�����
           mqtt_publish(&mymqtt,MY2PublishTopic,MainBuffer,MainLen, 0, 0,PublishedCb);//������Ϣ  ******************************* ��������Ϊ  listen1 
           
       }
          
}


/**
* @brief   ��������ɹ�

**/
void subscribedCb(int pdata)
{
	printf("\r\n�ɹ���������\r\n");
}

/**
* @brief   ��������ʧ��
**/
void failsubscribedCb(int pdata)
{
	printf("\r\n��������ʧ��\r\n");
}


/**
* @brief   �����ɹ�
**/
void PublishedCb()
{
	printf("\r\n�����ɹ�\r\n");
}



