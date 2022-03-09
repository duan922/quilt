/*
 * mqtt.h
 *
 *  Created on: 2020?3?13?
 *      Author: yang
 */

#ifndef APP_INCLUDE_DRIVER_MQTT_H_
#define APP_INCLUDE_DRIVER_MQTT_H_


#ifndef MQTT_C_//���û�ж���
#define MQTT_Ex_ extern
#else
#define MQTT_Ex_
#endif


#include "string.h"
#include "stm32f10x.h"
#include "mqtt_msg.h"
#include "BufferManage.h"


#define mqtt_timerout_default 3000      //������Ϣ��ʱʱ��
#define mqtt_timerout_send_default 3000 //������Ϣ��ʱʱ��

#define mqtt_send_buff_len 500  //MQTT���ݰ���󳤶�
#define send_buff_loop_len 1024 //����MQTT�������ݵ���󳤶�
#define send_buff_loop_m_len 10 //���������ݵ�����

typedef void (* MqttCallback)();

typedef void (* MqttCallback1)(int pdata);

typedef void (*MqttRecCallback)(const char* topic, uint32_t topic_len, const char *data, uint32_t lengh);

typedef struct mqtt
{
	char state;
	unsigned char *ptr;
	char mqtt_message_type;
	signed int mqtt_send_data_len;
	unsigned char mqtt_data_buff[mqtt_send_buff_len];//��������MQTT����
	unsigned char send_buff[mqtt_send_buff_len];//��ȡ��������ݴ洢������
	
	buff_manage_struct buff_manage_struct_t;//�������
	unsigned  char send_buff_loop[send_buff_loop_len];//�������ݵ�����
	uint32_t  send_buff_loop_m[send_buff_loop_m_len];//�����������
	
	signed int timer_out_cnt;//ͨ�Ź����еȴ�Ӧ��ʱ�ۼӱ���(�û�����Ҫ�޸�)
	signed int timer_out_send;//�������ݳ�ʱ
	uint16_t mqtt_message_id;
	MqttCallback connectCb;
	MqttCallback disconnectCb;
	MqttCallback PublishedCb;
	MqttCallback1 subscribedCb;
	MqttCallback1 failsubscribedCb;
	MqttRecCallback recCb;
	mqtt_connect_info_t mqtt_connect_info;
	
	/*��������ʹ��*/
	const char* topic;
  const char* data;
  uint16_t topic_length;
  uint16_t data_length;
	
	/*����������*/
	uint16_t KeepAliveTimeCnt;
	char KeepAliveSendFlage;
	uint16_t KeepAliveTimeOut;
	uint16_t KeepAliveSendCount;
} mqtt_t;


MQTT_Ex_ mqtt_t mymqtt;


void mqtt_init(mqtt_t *mqtt);
void mqtt_connect_reg(mqtt_t *mqtt,MqttCallback connectCb);
int  mqtt_connect_ack(unsigned char *buff);
void mqtt_disconnect_reg(mqtt_t *mqtt,MqttCallback disconnectCb);
void mqtt_received_reg(mqtt_t *mqtt,MqttRecCallback recCb);

void mqtt_time_data(mqtt_t *mqtt);
int mqtt_ping(mqtt_t *mqtt);
void mqtt_keep_alive(mqtt_t *mqtt);
void mqtt_keep_alive_init(mqtt_t *mqtt);

void mqtt_send_function(mqtt_t *mqtt);
void mqtt_read_function(mqtt_t *mqtt,unsigned char* buffer, uint16_t length);//�û���Ҫ������ģ����յ���MQTT���ݴ����˺�������

int mqtt_connect(mqtt_t *mqtt,unsigned char **data_ptr);
int mqtt_publish(mqtt_t *mqtt,unsigned char* topic,unsigned char* date, int data_length, int qos, int retain, MqttCallback PublishedCb);//������Ϣ
int mqtt_subscribe(mqtt_t *mqtt,unsigned  char* topic, uint8_t qos,MqttCallback1 subscribedCb,MqttCallback1 failsubscribedCb);//��������
int mqtt_into_buff(mqtt_t *mqtt);//�����ݲ��뻺��


#endif /* APP_INCLUDE_DRIVER_MQTT_H_ */
