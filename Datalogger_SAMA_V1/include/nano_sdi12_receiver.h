#pragma once
#include <Arduino.h>
#include "data_types.h"

void nanoReceiverBegin();
bool nanoReceiverRead(NanoSDI12Data &data);
