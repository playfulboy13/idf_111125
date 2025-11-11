#include "main.h"

const char *TAG="ESP32_NBN";

void app_main(void)
{
    nvs_flash_init();
    wifi_init_config();
    mqtt_app_start();

    xTaskCreate(Task1,"Task1",4096,NULL,5,NULL);
    
}