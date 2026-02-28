#include "status_broadcaster.h"
#include "mqttcomm.h"
#include "state.h"
#include "esp_log.h"
#include <stdio.h>

#define MQTT_TOPIC "funkbox/sensors"

static const char* TAG = "broadcaster";

void broadcaster_send_update(void) {
    struct mcu_data* data = state_get_current_state();
    char json[128];
    int len = snprintf(json, sizeof(json),
        "{\"oil_temp\":%d,\"oil_pres\":%.2f,\"gas_pres\":%.2f}",
        data->oil.temp,
        data->oil.preassure,
        data->gas.preassure);
    mqttcomm_publish(MQTT_TOPIC, json, len);
    ESP_LOGI(TAG, "Sent: %s", json);
}
