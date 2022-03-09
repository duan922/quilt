/**
  ******************************************************************************
  * @file    mqtt.c
  * @author  fengwu yang
  * @version V1.0.0
  * @date    2020/3/19
  * @brief   
  ******************************************************************************
	һ,ʹ��˵��
		1,�����³������1ms��ʱ���ж���
			mqtt_time_data(&mymqtt);
			
		2,�����³���ŵ�������
		while(1)
		{
			if(ģ����������MQTT������(TCP������))
			{
				mqtt_send_function(&mymqtt);//��ȡ���ͻ����MQTTЭ��
				mqtt_keep_alive(&mymqtt);//������������
				
				if(����������������)
				{
					mqtt_read_function(&mymqtt,���ݵ�ַ,���ݳ���);
				}
			}

		}
			
  ******************************************************************************
  */


#define MQTT_C_
#include <string.h>
#include "mqtt.h"
#include "mqtt_msg.h"
#include "stm32f10x.h"
#include "BufferManage.h"


//�滻�Լ��ķ��ͺ���:ctrl+F   ����:   Send function
#include "usart.h" //����ģ����ô���ͨ��

mqtt_t mymqtt;


/**
* @brief   ����MQTT����:�ѷ������ݸ�����ģ��ĺ������ڴ˴�
* @param   mqtt:  �ṹ��ָ��   Ĭ����д  &mymqtt
* @param   mqtt   ���͵�����
* @param   None
* @retval  None
* @warning None
* @example
**/
void mqtt_send_function(mqtt_t *mqtt)
{
	/*���Է�������*/
	if(mqtt->timer_out_cnt<=0 && mqtt->timer_out_send<=0)
	{
		BufferManageRead(&mqtt->buff_manage_struct_t,mqtt->send_buff,&mqtt->buff_manage_struct_t.SendLen);/*��ȡ������������*/
		
		if(mqtt->buff_manage_struct_t.SendLen>0)//��������Ҫ����
		{
			if(mqtt_get_type(mqtt->send_buff) == MQTT_MSG_TYPE_SUBSCRIBE){//���͵���Ϣ�Ƕ���
				mqtt->mqtt_message_id = mqtt_get_id(mqtt->send_buff, mqtt->buff_manage_struct_t.SendLen);//��ȡ��ϢID
				mqtt->mqtt_message_type = MQTT_MSG_TYPE_SUBSCRIBE;
				mqtt->timer_out_cnt = mqtt_timerout_default;//���ó�ʱʱ��
			}
			else if(mqtt_get_type(mqtt->send_buff) == MQTT_MSG_TYPE_PUBLISH){//���͵���Ϣ�Ƿ���
				if(mqtt_get_qos(mqtt->send_buff) == 1 || mqtt_get_qos(mqtt->send_buff) == 2)//��Ϣ�ȼ���1����2
				{
					mqtt->mqtt_message_id = mqtt_get_id(mqtt->send_buff, mqtt->buff_manage_struct_t.SendLen);//��ȡ��ϢID,�Ա�Ӧ��
					mqtt->mqtt_message_type = MQTT_MSG_TYPE_PUBLISH;
					mqtt->timer_out_cnt = mqtt_timerout_default;//���ó�ʱʱ��
				}
			}
			else if(mqtt_get_type(mqtt->send_buff) == MQTT_MSG_TYPE_PUBREL){//�ͻ����ϴη�������Ϣ�ȼ���2����Ϣ,����������PUBREC,�ͻ�����Ҫ����PUBREL
				mqtt->timer_out_cnt = mqtt_timerout_default;//���ó�ʱʱ��
			}
			//ʹ�ô��ڷ�������(��������ģ��):���滻�Լ��ķ��ͺ���
			//mqtt->send_buff:���͵�����
			//mqtt->buff_manage_struct_t.SendLen:���͵����ݳ���
			//Send function

			UsartOutStrIT(USART3,mqtt->send_buff,mqtt->buff_manage_struct_t.SendLen);//���滻�Լ��ķ��ͺ���

			//��������ķ��ͺ����Ժ�,����Ĭ�ϵȴ�3S,�ٷ�����һ������!
			//�������1:�����Լ��ķ������ݺ����Ժ�,�ڷ�����ɵĵط�д��: mymqtt.timer_out_send = 0; //�ǵð��� #include "mqtt.h"
			//�������2:���û�з�����ɵĵط�,����mqtt.h�����޸�  mqtt_timerout_send_default  ������Ĭ����ʱʱ�� ms
			
			mqtt->timer_out_send=mqtt_timerout_send_default;
		}
	}
}


/**
* @brief   ���մ���MQTT����:�û���Ҫ������ģ����յ���MQTT���ݴ����˺�������
* @param   mqtt:    �ṹ��ָ��    Ĭ����д  &mymqtt
* @param   buffer   ���յ�MQTT����
* @param   length   ���ݳ���
* @retval  None
* @warning None
* @example
**/
void mqtt_read_function(mqtt_t *mqtt,unsigned char* buffer, uint16_t length)
{
	uint8_t msg_type;
	uint8_t msg_qos;
	uint16_t msg_id;
	int DataLen=0;
	
	msg_type = mqtt_get_type(buffer);
	msg_qos = mqtt_get_qos(buffer);
	msg_id = mqtt_get_id(buffer, length);
	
	
	if(mqtt->mqtt_message_type == MQTT_MSG_TYPE_SUBSCRIBE){//�ϴη��͵��Ƕ�������
		mqtt_keep_alive_init(mqtt);//��ʼ������������
		//�ϴη��͵���Ϣ�Ƕ���
		if(msg_type == MQTT_MSG_TYPE_SUBACK){//��ȡӦ��
			mqtt->timer_out_cnt = 0;//ֹͣ��ʱ��ʱ��
			if(msg_id == mqtt->mqtt_message_id && ( buffer[length-1]&0xff) != 0x80 )
			{
				mqtt->mqtt_message_type = MQTT_MSG_TYPE_SUBACK;
				if(mqtt->subscribedCb!=NULL){
					mqtt->subscribedCb(mqtt->mqtt_message_id);
				}
			}
			else
			{
				mqtt->mqtt_message_type = 0;
				if(mqtt->failsubscribedCb!=NULL)mqtt->failsubscribedCb(mqtt->mqtt_message_id);
			}
		}
	}
	
	switch (msg_type)
	{
		case MQTT_MSG_TYPE_PUBLISH://���յ���Ϣ
			mqtt_keep_alive_init(mqtt);//��ʼ������������
			if (msg_qos == 1){//��Ϣ�ȼ���1,�����Ҫ���ص�PUBACK����
				mqtt->mqtt_send_data_len = mqtt_msg_puback(msg_id,&mqtt->ptr,mqtt->mqtt_data_buff,mqtt_send_buff_len);
			}
			else if (msg_qos == 2){//��Ϣ�ȼ���2,�����Ҫ���ص�PUBREC
				mqtt->mqtt_send_data_len = mqtt_msg_pubrec(msg_id,&mqtt->ptr,mqtt->mqtt_data_buff,mqtt_send_buff_len);
			}
			
			if (msg_qos == 1 || msg_qos == 2) {
					if(mqtt->mqtt_send_data_len >0 ){						
						BufferManageWrite(&mqtt->buff_manage_struct_t,mqtt->ptr,mqtt->mqtt_send_data_len,&DataLen);/*��Э�������뻺��*/
				}
			}
			
			//���ý��ջص�����
			if(mqtt->recCb){
				mqtt->topic_length = length;
				mqtt->topic = mqtt_get_publish_topic(buffer, &mqtt->topic_length);
				mqtt->data_length = length;
				mqtt->data = mqtt_get_publish_data(buffer, &mqtt->data_length);
				
				mqtt->recCb(mqtt->topic,mqtt->topic_length,mqtt->data,mqtt->data_length);
			}
			break;
			
		case MQTT_MSG_TYPE_PUBACK://�ͻ����ϴη�������Ϣ�ȼ���1����Ϣ,����������PUBACK,˵����Ϣ�Ѿ��ʹ�
			if(mqtt->mqtt_message_type == MQTT_MSG_TYPE_PUBLISH && mqtt->mqtt_message_id == msg_id){
				mqtt->mqtt_message_type = 0;
				mqtt->timer_out_cnt = 0;//ֹͣ��ʱ��ʱ��
				if(mqtt->PublishedCb){
					mqtt->PublishedCb();
				}
			}
			break;
		
		case MQTT_MSG_TYPE_PUBREC://�ͻ����ϴη�������Ϣ�ȼ���2����Ϣ,����������PUBREC,�ͻ�����Ҫ����PUBREL
			mqtt->mqtt_send_data_len = mqtt_msg_pubrel(msg_id,&mqtt->ptr,mqtt->mqtt_data_buff,mqtt_send_buff_len);
			mqtt->timer_out_cnt = 0;
			BufferManageWrite(&mqtt->buff_manage_struct_t,mqtt->ptr,mqtt->mqtt_send_data_len,&DataLen);/*��Э�������뻺��*/
			break;
		case MQTT_MSG_TYPE_PUBCOMP://�ͻ����ϴη�������Ϣ�ȼ���2����Ϣ,����������PUBREC,�ͻ��˷�����PUBREL,��������󷵻�PUBCOMP,˵����Ϣ�Ѿ��ʹ�
			if (mqtt->mqtt_message_type == MQTT_MSG_TYPE_PUBLISH && mqtt->mqtt_message_id == msg_id) {
				mqtt->mqtt_message_type = 0;
				mqtt->timer_out_cnt = 0;//ֹͣ��ʱ��ʱ��
				if(mqtt->PublishedCb){
					mqtt->PublishedCb();
				}
			}
			break;
		case MQTT_MSG_TYPE_PUBREL://�ͻ����յ�����Ϣ�ȼ�Ϊ2����Ϣ,ͬʱ�ظ���PUBREC,����������PUBREL,�ͻ��������Ҫ����PUBCOMP
			mqtt->mqtt_send_data_len = mqtt_msg_pubcomp(msg_id,&mqtt->ptr,mqtt->mqtt_data_buff,mqtt_send_buff_len);
			
			BufferManageWrite(&mqtt->buff_manage_struct_t,mqtt->ptr,mqtt->mqtt_send_data_len,&DataLen);/*��Э�������뻺��*/
			break;
		case MQTT_MSG_TYPE_PINGRESP://���յ�����������
				mqtt_keep_alive_init(mqtt);//��ʼ������������
			break;
		
		default:
			break;
	}
}


/**
* @brief   ��ʱ��ѵ����(�ú�����Ҫ����1ms��ʱ��)
* @param   mqtt:�ṹ��ָ�� Ĭ����д  &mymqtt
* @param   None
* @retval  None
* @warning None
* @example
**/
void mqtt_time_data(mqtt_t *mqtt)
{
	/*MQTTͨ�ų�ʱ���*/
	if(mqtt->timer_out_cnt>0){
		mqtt->timer_out_cnt --;
	}
	
	/*�������ݳ�ʱ���,Ϊȷ��������һ���ٷ�����һ��*/
	if(mqtt->timer_out_send>0){
		mqtt->timer_out_send--;
	}
	
	/*MQTT������*/
	if(mqtt->KeepAliveSendCount>0){
		mqtt->KeepAliveTimeOut++;
	}
	else{
		mqtt->KeepAliveTimeOut=0;
	}
	
	mqtt->KeepAliveTimeCnt++;
	if(mqtt->KeepAliveTimeCnt/1000 >= mqtt->mqtt_connect_info.keepalive)//��ʱ�䷢��������
	{
		mqtt->KeepAliveSendFlage = 1;
	}
}



/**
* @brief  ������������
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void mqtt_keep_alive(mqtt_t *mqtt)
{
	if(mqtt->KeepAliveSendFlage)//��Ҫ����������
	{
		mqtt_ping(mqtt);
	}
	if(mqtt->KeepAliveTimeOut >5000)//����������ʱ��û�з���Ӧ��
	{
		mqtt->KeepAliveTimeOut = 0;
		mqtt_ping(mqtt);
	}
}


/**
* @brief  ��ʼ������������
* @param  
* @param  None
* @param  None
* @retval None
* @example 
**/
void mqtt_keep_alive_init(mqtt_t *mqtt)
{
//	mqtt->KeepAliveTimeCnt=0;
	mqtt->KeepAliveSendCount=0;
	mqtt->KeepAliveTimeOut=0;
}

/**
* @brief   ��ʼ��MQTT�ṹ��ָ��   
* @param   mqtt:�ṹ��ָ��   
* @param   None
* @retval  None
* @warning None
* @example
**/
void mqtt_init(mqtt_t *mqtt)
{
	BufferManageCreate(&mqtt->buff_manage_struct_t, mqtt->send_buff_loop, send_buff_loop_len, mqtt->send_buff_loop_m, send_buff_loop_m_len*4);
	
	mqtt->mqtt_message_type=0;
	mqtt->mqtt_send_data_len=0;
	mqtt->state=0;
	mqtt->timer_out_cnt=0;
	mqtt_keep_alive_init(mqtt);
}



/**
* @brief   �������MQTTЭ��
* @param   mqtt   
* @param   data_ptr:���ش��������ָ��  
* @retval  ���ݸ���
* @warning None
* @example
**/
int mqtt_connect(mqtt_t *mqtt,unsigned char **data_ptr)
{
	return mqtt_msg_connect(&mqtt->mqtt_connect_info,data_ptr,mqtt->mqtt_data_buff,mqtt_send_buff_len);
}



/**
* @brief  �ж��Ƿ�������MQTT
* @param  ���������ص�����
* @param  
* @retval 0 ���ӳɹ�
* @example 
**/
int  mqtt_connect_ack(unsigned char *buff)
{
	if(mqtt_get_type(buff) == MQTT_MSG_TYPE_CONNACK)
	{
		return mqtt_get_connect_ret_code(buff);
	}
	return -1;
}


/**
* @brief   ע�����Ӻ���
* @param   mqtt   
* @param   connectedCb   
* @retval  None
* @warning None
* @example
**/
void mqtt_connect_reg(mqtt_t *mqtt,MqttCallback connectCb)
{
	mqtt->connectCb = connectCb;
}

/**
* @brief   ע��Ͽ����Ӻ���
* @param   mqtt   
* @param   connectedCb   
* @retval  None
* @warning None
* @example
**/
void mqtt_disconnect_reg(mqtt_t *mqtt,MqttCallback disconnectCb)
{
	mqtt->disconnectCb = disconnectCb;
}


/**
* @brief   ע��MQTT���յ����ݻص�����
* @param   mqtt   
* @param   disconnectedCb   
* @retval  None
* @warning None
* @example
**/
void mqtt_received_reg(mqtt_t *mqtt,MqttRecCallback recCb)
{
	mqtt->recCb = recCb;
}



/**
* @brief   ��������
* @param   mqtt:mqtt_t�ṹ�����   
* @param   topic:���ĵ�����
* @param   qos:��Ϣ�ȼ�
* @param   subscribedCb:���ĳɹ��ص�
* @param   failsubscribedCb:����ʧ�ܻص�
* @param   None
* @retval  0:Success  1:������ݴ���  2:������
* @warning None
* @example
**/
int mqtt_subscribe(mqtt_t *mqtt,unsigned  char* topic, uint8_t qos,MqttCallback1 subscribedCb,MqttCallback1 failsubscribedCb)
{
	mqtt->mqtt_send_data_len = mqtt_msg_subscribe_topic(topic,qos,&mqtt->ptr,mqtt->mqtt_data_buff,mqtt_send_buff_len);/*�������Э��*/
	mqtt->subscribedCb = subscribedCb;
	mqtt->failsubscribedCb = failsubscribedCb;
	return mqtt_into_buff(mqtt);
}


/**
* @brief   ������Ϣ
* @param   mqtt:mqtt_t�ṹ�����   
* @param   topic:����������
* @param   date: ���͵�����
* @param   data_length:���ݳ���
* @param   qos:��Ϣ�ȼ�
* @param   retain:�Ƿ���Ҫ������������Ϣ
* @param   PublishedCb:�����ɹ��ص�(ֻ��qos=1/2ʱ��Ч)
* @retval  0:Success  1:������ݴ���  2:������
* @warning None
* @example
**/
int mqtt_publish(mqtt_t *mqtt,unsigned char* topic,unsigned char* date, int data_length, int qos, int retain, MqttCallback PublishedCb)
{
	mqtt->mqtt_send_data_len = mqtt_msg_publish(topic,date,data_length,qos,retain,&mqtt->ptr,mqtt->mqtt_data_buff,mqtt_send_buff_len);/*�������Э��*/
	mqtt->PublishedCb = PublishedCb;
	return mqtt_into_buff(mqtt);
}


/**
* @brief   ��������������
* @param   mqtt:mqtt_t�ṹ�����
* @param   None
* @param   None
* @param   None
* @param   None
* @param   None
* @retval  0:Success  1:������ݴ���  2:������
* @warning None
* @example
**/
int mqtt_ping(mqtt_t *mqtt)
{
	int len;
	mqtt->mqtt_send_data_len = mqtt_msg_pingreq(&mqtt->ptr,mqtt->mqtt_data_buff,mqtt_send_buff_len);/*���Э��*/
	len = mqtt_into_buff(mqtt);
	
	if(len==0){
		mqtt->KeepAliveTimeCnt=0;
		mqtt->KeepAliveSendFlage=0;
		mqtt->KeepAliveSendCount++;
		if(mqtt->KeepAliveSendCount>=4){
			mqtt->KeepAliveSendCount=0;
			if(mqtt->disconnectCb){
				mqtt->disconnectCb();
			}
		}
	}
	return len;
}



/**
* @brief   �����ݲ��뻺��
* @param   mqtt   
* @retval  0:Success  1:������ݴ���  2:������
* @warning None
* @example
**/
int mqtt_into_buff(mqtt_t *mqtt)
{
	int len;
	if(mqtt->mqtt_send_data_len >0 )
	{
		BufferManageWrite(&mqtt->buff_manage_struct_t,mqtt->ptr,mqtt->mqtt_send_data_len,&len);/*��Э�������뻺��*/

		if(len==0){
			return 0;
		}
		else
		{
			return -2;
		}			
	}
	else
	{
		return -1;
	}
}






