idf_component_register(SRCS "main.c"
INCLUDE_DIRS "."
REQUIRES driver freertos mqtt nvs_flash json esp_http_server esp_wifi
EMBED_TXTFILES "hivemq_root_ca.pem")