#include <Arduino.h>
#include "mqtt_client.h"

unsigned long lastPublish = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("====================================");
  Serial.println("Stage MQTT - W5500 + EMQX TLS");
  Serial.println("====================================");

  if (!mqttBegin()) {
    Serial.println("Error inicializando MQTT.");
  } else {
    Serial.println("MQTT inicializado correctamente.");
  }
}

void loop() {
  mqttLoop();

  if (millis() - lastPublish >= 5000) {
    lastPublish = millis();

    float nivel = random(100, 300) / 10.0;
    float bateria = random(115, 130) / 10.0;

    String payload = "{";
    payload += "\"equipo\":\"DATALOGGER_SAMA_STAGE_MQTT\",";
    payload += "\"nivel\":" + String(nivel, 2) + ",";
    payload += "\"bateria\":" + String(bateria, 2);
    payload += "}";

    Serial.print("Publicando MQTT: ");
    Serial.println(payload);

    bool ok = mqttPublishDefault(payload);

    Serial.print("Estado publicacion: ");
    Serial.println(ok ? "OK" : "ERROR");
  }
}