#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <analog_reader.h>
#include <esp_log.h>
#include <status_broadcaster.h>
#include <ads111x.h>
#include <state.h>
#include <esp_err.h>
#define I2C_PORT 0

#define GAIN ADS111X_GAIN_6V144
#define R_OHM 220

char* ANALOG_TAG = "analogue_read";

i2c_dev_t device;
float gain_val;

int res_values[] = { 44864 , 33676, 25524, 19525, 15067, 11724, 9195, 7266, 5784, 4636, 3740, 3037, 2480, 2038, 1683, 1398, 1167, 978, 825, 698, 594, 507, 435, 374, 323, 280, 244, 213, 186, 164, 144, 128, 113, 100, 90, 80, 72 };

float read_value() {
    int16_t raw = 0;
    if (ads111x_get_value(&device, &raw) == ESP_OK) {
        return gain_val / ADS111X_MAX_VALUE * raw;
    } else {
        return 0;
    }   
}

int calculate_temperature(int resistance) {
    bool entry_found = false;
    int upper_index = 0;
    int lower_index = 0;
    int arr_length = sizeof(res_values) / sizeof(int);
    for (int i = 0; i < arr_length; i++) {
        upper_index = i;
        lower_index = i-1; 
        if(resistance > res_values[i]) {
            entry_found = true;
            break;
        }
    }

    if (lower_index < 0) {
        lower_index = 0;
    }
    
    if (!entry_found) {
        upper_index = lower_index = arr_length - 1;
    }
    
    if (lower_index == upper_index) {
        return lower_index * 5 - 40;
    }

    int range_in_bound = res_values[lower_index] - res_values[upper_index];
    int measurement_in_bound = res_values[lower_index] - resistance;

    ESP_LOGD(ANALOG_TAG, "Lower Tag: [%d], Upper IDX [%d], range: [%d], measrurement [%d]", lower_index, upper_index, range_in_bound, measurement_in_bound);

    return (lower_index * 5 - 40) + (float)(measurement_in_bound / (float)range_in_bound)*5;
}


void analogue_read_oil_temp(float vref) {
    ESP_ERROR_CHECK(ads111x_set_input_mux(&device, ADS111X_MUX_1_GND));
    vTaskDelay(50 / portTICK_PERIOD_MS);
    float vmeasure = read_value();
    ESP_LOGD(ANALOG_TAG, "ADC value Oil Temp: %.04f volts", vmeasure);

    float vres = vref - vmeasure;
    float curr = vres / R_OHM;
    ESP_LOGD(ANALOG_TAG, "Curr value: %f A", curr);

    int temp_res = vmeasure / curr;
    ESP_LOGD(ANALOG_TAG, "Resistor value: %d ohm", temp_res);
    ESP_LOGD(ANALOG_TAG, "Temp: %d °C", calculate_temperature(temp_res));
    state_set_oil_temp(calculate_temperature(temp_res));
}

void analogue_read_oil_pres(float vref) {
    ESP_ERROR_CHECK(ads111x_set_input_mux(&device, ADS111X_MUX_0_GND));
    vTaskDelay(50 / portTICK_PERIOD_MS);
    float vmeasure = read_value();
    ESP_LOGD(ANALOG_TAG, "ADC value Oil Pres: %.04f volts", vmeasure);

    int kpas = (vmeasure - 0.1*vref) / (vref * 0.0008);
    ESP_LOGD(ANALOG_TAG, "ADC value Pres: %d kPa", kpas);
    if (kpas < 0) {
        state_set_oil_pres(0);
    } else {
        state_set_oil_pres(kpas / (double)100.0);
    }
}

static const int r1 = 47;
static const int r_sensor_min = 10;
static const int r_sensor_max = 180;
static const double p_sensor_min = 0.0;
static const double p_sensor_max = 10.0;

void analogue_read_gas_pres(float uRef) {
    ESP_ERROR_CHECK(ads111x_set_input_mux(&device, ADS111X_MUX_3_GND));
    vTaskDelay(50 / portTICK_PERIOD_MS);
    float u2 = read_value();
    ESP_LOGD(ANALOG_TAG, "ADC value GAS Pres: %.04f volts", u2);

    int r2 = (u2 * r1) / (uRef - u2);
    ESP_LOGD(ANALOG_TAG, "R2 Value: %d ohm", r2);

    int zero_adjusted_r2 = r2 - r_sensor_min;
    // value between 0..1
    double relative_progression = zero_adjusted_r2 / (double)(r_sensor_max - r_sensor_min);
    ESP_LOGD(ANALOG_TAG, "Rel Progression: %f", relative_progression);

    double gas_pres = p_sensor_min + ((p_sensor_max - p_sensor_min)*relative_progression);
    ESP_LOGD(ANALOG_TAG, "GAS value Pres: %fbar", gas_pres);
    state_set_gas_pres(gas_pres);
}

void measure() {
    bool simulator = false;
    esp_err_t ret = ads111x_init_desc(&device, ADS111X_ADDR_GND, I2C_PORT, GPIO_NUM_32, GPIO_NUM_33);
    
    ret |= ads111x_set_mode(&device, ADS111X_MODE_CONTINUOUS);    // Continuous conversion mode
    ret |= ads111x_set_data_rate(&device, ADS111X_DATA_RATE_860); // 32 samples per second
    ret |= ads111x_set_gain(&device, GAIN);

    if(ret != ESP_OK) {
        simulator = true;
        ESP_LOGD(ANALOG_TAG, "DAC not connected, going to sim mode");
    }

    while(!simulator) {
        ESP_ERROR_CHECK(ads111x_set_input_mux(&device, ADS111X_MUX_2_GND));
        vTaskDelay(50 / portTICK_PERIOD_MS);
        float vref = read_value();
        ESP_LOGD(ANALOG_TAG, "ADC value Ref: %.04f volts", vref);

        analogue_read_oil_temp(vref);
        analogue_read_oil_pres(vref);
        analogue_read_gas_pres(vref);
        broadcaster_send_update();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    while (simulator) {
        state_set_oil_temp(110 + rand() % 5);
        state_set_oil_pres(4.3 + (double)(rand() % 10 / 10.0));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void analog_reader_start() {
        gain_val = ads111x_gain_values[GAIN];
        ESP_LOGD(ANALOG_TAG, "beep");
        ESP_ERROR_CHECK(i2cdev_init());

        xTaskCreate(measure, "ads111x", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL);

}
