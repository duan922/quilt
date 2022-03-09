#ifndef __MAIN_H_
#define __MAIN_H_
#include <stm32f10x.h>

#ifndef __MAIN_C_
#define __MAIN_EXT_ extern
#else
#define __MAIN_EXT_
#endif


//��������ʹ��
#define MainBufferLen 200
__MAIN_EXT_ unsigned char MainBuffer[MainBufferLen];//��������,ȫ��ͨ��
__MAIN_EXT_ u32  MainLen;      //ȫ��ͨ�ñ���
__MAIN_EXT_ unsigned char *MainString;    //ȫ��ͨ�ñ���



__MAIN_EXT_ unsigned char IP[55];//IP��ַ/����
__MAIN_EXT_ unsigned char Port[7];//�˿ں�
__MAIN_EXT_ unsigned char MQTTid[50];//ClientID
__MAIN_EXT_ unsigned char MQTTUserName[20];//�û���
__MAIN_EXT_ unsigned char MQTTPassWord[50];//����
__MAIN_EXT_ unsigned char MQTTkeepAlive;//������ʱ��
__MAIN_EXT_ unsigned char MQTTPublishTopic[30];//�洢MQTT����������
__MAIN_EXT_ unsigned char MQTTWillMessage[50];//������Ϣ
__MAIN_EXT_ unsigned char MQTTWillQos;//��Ϣ�ȼ�
__MAIN_EXT_ unsigned char MQTTWillRetained;//�Ƿ���Ҫ��������¼



#endif
