#pragma once
#include <Arduino.h>
#include "data_types.h"

bool ads1115Begin();
bool ads1115ReadChannel(uint8_t canal, float &voltaje);
bool ads1115ReadAll(ADS1115Data &data);
