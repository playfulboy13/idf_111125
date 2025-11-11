#include "mqtt.h"

extern uint8_t hivemq_root_ca_pem_start[] asm("_binary_hivemq_root_ca_pem_start");
extern uint8_t hivemq_root_ca_pem_stop[] asm("_binary_hivemq_root_ca_pem_stop");

bool wifi_connected=false;
bool mqtt_connected=false;

QueueHandle_t mqtt_msg_queue=NULL;
esp_mqtt_client_handle_t global_client;


static void wifi_event_handler(void *arg,esp_event_base_t event_base,int32_t event_id,void *event_data)
{
    if(event_base==WIFI_EVENT&&event_id==WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
        wifi_connected=false;
    }
    else if(event_base==IP_EVENT&&event_id==IP_EVENT_STA_GOT_IP)
    {
        ESP_LOGI(TAG,"CONNECTED!\r\n");
        wifi_connected=true;
    }
    else if(event_base==WIFI_EVENT&&event_id==WIFI_EVENT_STA_STOP)
    {
        esp_wifi_connect();
        ESP_LOGI(TAG,"RECONNECTING");
        wifi_connected=false;
    }
    else if(event_base==WIFI_EVENT&&event_id==WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();
        wifi_connected=false;
    }
}

void wifi_init_config(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t conf=WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t wifi_config={
        .sta={
            .ssid=WIFI_SSID,
            .password=WIFI_PASS,
        },
    };
    esp_event_handler_instance_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&wifi_event_handler,NULL,NULL);
    esp_event_handler_instance_register(IP_EVENT,IP_EVENT_STA_GOT_IP,&wifi_event_handler,NULL,NULL);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA,&wifi_config);
    esp_wifi_start();
}

static void mqtt_event_handler(void *arg,esp_event_base_t event_base,int32_t event_id,void *event_data)
{
    esp_mqtt_event_handle_t event=event_data;
    esp_mqtt_client_handle_t client=event->client;

    switch(event->event_id)
    {
        case MQTT_EVENT_CONNECTED:
        {
            global_client=client;
            esp_mqtt_client_subscribe(client,"namban123/control",1);
            break;
        }
        case MQTT_EVENT_DATA:
        {
            mqtt_msg_t msg;
            memset(&msg,0,sizeof(msg));

            snprintf(msg.topic,sizeof(msg.topic),"%.*s",event->topic_len,event->topic);
            snprintf(msg.data,sizeof(msg.data),"%.*s",event->data_len,event->data);
            if(mqtt_msg_queue!=NULL)
            {
                xQueueSendFromISR(mqtt_msg_queue,&msg,NULL);
            }
            break;
        }
        default:
        {
            ESP_LOGI(TAG,"MQTT ID: %d",event->event_id);
            break;
        }
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_config={
        .broker={
            .address.uri="mqtts://1f7d050368244daa8dcc8c94f6887a39.s1.eu.hivemq.cloud",
            .address.port=8883,
            .verification.certificate=(const char*)(hivemq_root_ca_pem_start),
        },
        .credentials={
            .client_id="esp32_kit",
            .username="namban_123",
            .authentication.password="Namban123@",
        },
        .session.last_will={
            .topic="namban123/status",
            .msg="offline",
            .qos=1,
            .retain=true,
        },
    };
    esp_mqtt_client_handle_t client=esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client,ESP_EVENT_ANY_ID,&mqtt_event_handler,NULL);
    esp_mqtt_client_start(client);
    mqtt_msg_queue=xQueueCreate(10,sizeof(mqtt_msg_t));
}

void TaskMqttPublish(void *pvParameters)
{
    char buffer[64];
    
    while(1)
    {
        snprintf(buffer,sizeof(buffer),"HELLO BROKER FROM ESP32!");
        esp_mqtt_client_publish(global_client,"namban123/test_topic",buffer,0,0,false);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}