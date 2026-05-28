#pragma once
#include <Arduino.h>
#include "data_types.h"

bool ds3231Read(DateTimeData &dt);
void ds3231Adjust(int year, int month, int day, int hour, int minute, int second);
String formatTimestamp(const DateTimeData &dt);
