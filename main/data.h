#pragma once
#include "stdbool.h"
#include "inttypes.h"

struct car_sensor {
    int temp;
    double preassure;
};

struct mcu_data {
    struct car_sensor water;
    struct car_sensor oil;
    struct car_sensor gas;
};

