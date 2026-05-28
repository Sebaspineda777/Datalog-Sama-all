#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"
#include "nano_sdi12_receiver.h"

static HardwareSerial nanoSerial(2);

void nanoReceiverBegin() {
  nanoSerial.begin(NANO_BAUD, SERIAL_8N1, NANO_RX, NANO_TX);
}

bool nanoReceiverRead(NanoSDI12Data &data) {
  data.available = false;
  data.jsonOk = false;

  if (!nanoSerial.available()) return false;

  String line = nanoSerial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return false;

  data.raw = line;
  data.available = true;

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, line);
  if (err) {
    data.jsonOk = false;
    return true;
  }

  data.waterPotentialKPa = doc["wp"] | 0.0f;
  data.temperaturaC = doc["temp"] | 0.0f;
  data.jsonOk = true;
  return true;
}
