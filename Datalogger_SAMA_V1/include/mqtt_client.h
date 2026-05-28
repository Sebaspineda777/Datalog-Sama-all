#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>

bool mqttBegin();
bool mqttLoop();
bool mqttIsConnected();
bool mqttPublish(const char* topic, const String& payload);
bool mqttPublishDefault(const String& payload);

#endif