#pragma once

#include <stdio.h>
#include <stdbool.h>
#include "data.h"

void state_setup();

void state_set_oil_temp(int temp);
void state_set_oil_pres(double pres);
void state_set_gas_pres(double pres);

struct mcu_data* state_get_current_state();

