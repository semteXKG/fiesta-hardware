#include "state.h"
#include "esp_log.h"

struct mcu_data data;

void state_setup() {
}

void state_set_oil_temp(int temp) {
    data.oil.temp = temp;
}

void state_set_oil_pres(double pres) {
    data.oil.preassure = pres;
}

void state_set_gas_pres(double pres) {
    data.gas.preassure = pres;
}

struct mcu_data* state_get_current_state() {
    return &data;
}

