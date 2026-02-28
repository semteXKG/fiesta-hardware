#pragma once

#include <esp_netif_types.h>

void mqttcomm_start(esp_netif_t* netif);
int mqttcomm_publish(const char* topic, const char* data, int len);
