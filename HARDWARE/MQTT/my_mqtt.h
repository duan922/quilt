#ifndef __MY_MQTT_H
#define __MY_MQTT_H
#include "sys.h"

extern u8 MQTT_login;

u8 mqtt_connect(void);
u8 my_mqttPublish(char *sub,char *sp);
u8 my_subscribe(char *sp);
void MQTT_system(void);

#endif
