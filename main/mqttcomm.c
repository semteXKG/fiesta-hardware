#include "mqttcomm.h"
#include "led_status.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "lwip/inet.h"

static const char* TAG = "mqttcomm";
static esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = event_data;
    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");
            led_status_set_solid();
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGE(TAG, "MQTT disconnected, retrying in ~5s...");
            led_status_set_flash();
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT error, transport type: %d", event->error_handle->error_type);
            led_status_set_flash();
            break;
        default:
            break;
    }
}

void mqttcomm_start(esp_netif_t* netif) {
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(netif, &ip_info);

    char gw_ip[16];
    inet_ntoa_r(ip_info.gw.addr, gw_ip, sizeof(gw_ip));

    char broker_uri[32];
    snprintf(broker_uri, sizeof(broker_uri), "mqtt://%s", gw_ip);

    ESP_LOGI(TAG, "Connecting to MQTT broker at %s", broker_uri);

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = broker_uri,
        .network.reconnect_timeout_ms = 5000,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    led_status_set_flash();
    esp_mqtt_client_start(client);
}

int mqttcomm_publish(const char* topic, const char* data, int len) {
    if (client == NULL) {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return -1;
    }
    return esp_mqtt_client_publish(client, topic, data, len, 1, 0);
}
