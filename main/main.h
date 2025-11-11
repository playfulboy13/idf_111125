#ifndef _MAIN_H
#define _MAIN_H

extern const char *TAG;

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "driver/gpio.h"
#include "driver/i2c_master.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "mqtt_client.h"


#include "user_task.h"
#include "mqtt.h"

#endif