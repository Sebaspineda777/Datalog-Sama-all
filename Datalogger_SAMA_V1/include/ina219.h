#pragma once
#include <Arduino.h>
#include "data_types.h"

bool ina219Begin();
bool ina219Read(INA219Data &data);
