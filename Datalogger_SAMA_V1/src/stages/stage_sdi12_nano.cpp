#include <Arduino.h>
#include "nano_sdi12_receiver.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Etapa: ESP32 recibiendo SDI-12 desde Nano por UART2 ===");
  nanoReceiverBegin();
}

void loop() {
  NanoSDI12Data data;
  if (nanoReceiverRead(data)) {
    Serial.println("RX raw: " + data.raw);
    if (!data.jsonOk) {
      Serial.println("ERROR: JSON invalido");
      return;
    }
    Serial.println("------------------------------");
    Serial.printf("Water Potential: %.1f kPa\n", data.waterPotentialKPa);
    Serial.printf("Temperatura: %.2f C\n", data.temperaturaC);
    Serial.println("------------------------------");
  }
}
