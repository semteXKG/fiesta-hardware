#include "led_status.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define LED_GPIO GPIO_NUM_8

static esp_timer_handle_t flash_timer = NULL;
static bool led_on = false;

static void flash_timer_cb(void* arg) {
    led_on = !led_on;
    gpio_set_level(LED_GPIO, led_on ? 1 : 0);
}

void led_status_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(LED_GPIO, 0);

    const esp_timer_create_args_t timer_args = {
        .callback = &flash_timer_cb,
        .name = "led_flash"
    };
    esp_timer_create(&timer_args, &flash_timer);
}

void led_status_set_solid(void) {
    if (esp_timer_is_active(flash_timer)) {
        esp_timer_stop(flash_timer);
    }
    led_on = true;
    gpio_set_level(LED_GPIO, 1);
}

void led_status_set_flash(void) {
    if (!esp_timer_is_active(flash_timer)) {
        esp_timer_start_periodic(flash_timer, 250000); // 250ms toggle = ~2Hz
    }
}

void led_status_off(void) {
    if (esp_timer_is_active(flash_timer)) {
        esp_timer_stop(flash_timer);
    }
    led_on = false;
    gpio_set_level(LED_GPIO, 0);
}
