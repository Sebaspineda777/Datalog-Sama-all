#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include "config.h"
#include "sht31.h"
#include "ds3231.h"
#include "sd_logger.h"
#include "ads1115.h"
#include "ina219.h"
#include "oled_display.h"
#include "nano_sdi12_receiver.h"
#include "mqtt_client.h"

unsigned long lastLog = 0;
NanoSDI12Data lastSDI12;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=================================================");
  Serial.println("FULL MQTT - Datalogger SAMA");
  Serial.println("Sensores + SD + W5500 + MQTT TLS");
  Serial.println("=================================================");

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(I2C_CLOCK_HZ);

  // Ajustar una sola vez si se requiere y volver a comentar:
  // ds3231Adjust(2026, 5, 7, 10, 30, 0);

  Serial.println(ads1115Begin() ? "ADS1115 OK" : "ADS1115 ERROR");
  Serial.println(ina219Begin() ? "INA219 OK" : "INA219 ERROR");
  Serial.println(oledBegin() ? "OLED OK" : "OLED ERROR");

  if (!sdLoggerBegin()) {
    Serial.println("ERROR: SD no inicializa");
    oledShowError("SD ERROR");
    while (true) {
      delay(1000);
    }
  }

  sdLoggerCreateHeader(CSV_FILE);

  nanoReceiverBegin();

  if (!mqttBegin()) {
    Serial.println("ERROR: MQTT no inicializa");
    oledShowError("MQTT ERROR");
  } else {
    Serial.println("MQTT OK");
  }

  Serial.println("Sistema listo. Registro cada 30 segundos.");
}

void loop() {
  mqttLoop();

  // Captura asincronica del dato recibido desde Nano/SDI-12 si llega entre registros.
  NanoSDI12Data incoming;

  if (nanoReceiverRead(incoming)) {
    lastSDI12 = incoming;
    Serial.println("RX Nano/SDI-12: " + incoming.raw);
  }

  if (millis() - lastLog < LOG_INTERVAL_MS) {
    return;
  }

  lastLog = millis();

  SHT31Data sht;
  DateTimeData dt;
  ADS1115Data ads;
  INA219Data ina;

  sht31Read(sht);
  ds3231Read(dt);
  ads1115ReadAll(ads);
  ina219Read(ina);

  String ts = formatTimestamp(dt);

  String payload = "{";
payload += "\"ts\":\"" + ts + "\",";                                  // Timestamp
payload += "\"vin\":" + String(ina.busVoltageV, 3) + ",";             // Voltaje de entrada
payload += "\"icons\":" + String(ina.currentmA, 2) + ",";             // Corriente de consumo
payload += "\"temp\":" + String(sht.temperaturaC, 2) + ",";           // Temperatura SHT31
payload += "\"hum\":" + String(sht.humedadRH, 2) + ",";               // Humedad SHT31
payload += "\"a0\":" + String(ads.voltajeA0, 4) + ",";                // ADS1115 A0
payload += "\"a1\":" + String(ads.voltajeA1, 4) + ",";                // ADS1115 A1
payload += "\"sdi_temp\":" + String(lastSDI12.temperaturaC, 2) + ","; // SDI-12 temperatura
payload += "\"sdi_wp\":" + String(lastSDI12.waterPotentialKPa, 1);    // SDI-12 water potential
payload += "}";

  Serial.println();
  Serial.println("========== REGISTRO FULL MQTT ==========");
  Serial.println("Timestamp: " + ts);

  Serial.printf("SHT31: %.2f C, %.2f %%RH [%s]\n",
                sht.temperaturaC,
                sht.humedadRH,
                sht.ok ? "OK" : "ERROR");

  Serial.printf("ADS A0: %.4f V, %.3f mA, %.2f %%\n",
                ads.voltajeA0,
                ads.corrienteA0_mA,
                ads.porcentaje4_20);

  Serial.printf("ADS A1/A2/A3: %.4f / %.4f / %.4f V\n",
                ads.voltajeA1,
                ads.voltajeA2,
                ads.voltajeA3);

  Serial.printf("INA219: Vbus %.3f V, I %.2f mA, P %.2f mW\n",
                ina.busVoltageV,
                ina.currentmA,
                ina.powermW);

  Serial.printf("SDI12/Nano: wp %.1f kPa, temp %.2f C, raw [%s]\n",
                lastSDI12.waterPotentialKPa,
                lastSDI12.temperaturaC,
                lastSDI12.raw.c_str());

  Serial.print("Payload MQTT: ");
  Serial.println(payload);

  oledShowFull(sht, ina, ads);

  bool okSD = sdLoggerAppendFull(CSV_FILE, ts, sht, ads, ina, lastSDI12);
  Serial.println(okSD ? "Guardado SD: OK" : "Guardado SD: ERROR");

  File archivoPayload = SD.open("/mqtt_payloads.txt", FILE_APPEND);

  if (archivoPayload) {
    archivoPayload.println(payload);
    archivoPayload.close();
    Serial.println("Payload guardado en SD: OK");
  } else {
    Serial.println("Payload guardado en SD: ERROR");
  }

  bool mqttOK = mqttPublishDefault(payload);

  Serial.print("Publicacion MQTT: ");
  Serial.println(mqttOK ? "OK" : "ERROR");

  Serial.println("========================================");
}