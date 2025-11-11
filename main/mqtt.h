#ifndef _MQTT_H
#define _MQTT_H

#include "main.h"

#define WIFI_SSID "Ha Quan 2.4G"
#define WIFI_PASS "namban123@"

#define MQTT_MAX_MSG_LEN 128


typedef struct
{
    char topic[64];
    char data[MQTT_MAX_MSG_LEN];
}mqtt_msg_t;

extern QueueHandle_t mqtt_msg_queue;
extern esp_mqtt_client_handle_t global_client;

extern bool wifi_connected;
extern bool mqtt_connected;

void wifi_init_config(void);
void mqtt_app_start(void);

void TaskMqttPublish(void *pvParameters);



#endif
