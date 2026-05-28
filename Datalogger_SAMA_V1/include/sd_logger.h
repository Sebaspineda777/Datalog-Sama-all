#pragma once
#include <Arduino.h>
#include "data_types.h"

bool sdLoggerBegin();
bool sdLoggerCreateHeader(const char* path);
bool sdLoggerAppendFull(const char* path, const String &timestamp,
                        const SHT31Data &sht, const ADS1115Data &ads,
                        const INA219Data &ina, const NanoSDI12Data &sdi12);
