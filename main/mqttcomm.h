#pragma once

void mqttcomm_start(void);
int mqttcomm_publish(const char* topic, const char* data, int len, int qos, int retain);
