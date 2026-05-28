#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "sht31.h"
#include "ds3231.h"
#include "sd_logger.h"

unsigned long lastLog = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Etapa: SHT31 + DS3231 + microSD ===");

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(I2C_CLOCK_HZ);

  // Ajustar una sola vez si se requiere y volver a comentar:
  // ds3231Adjust(2026, 5, 7, 10, 30, 0);

  if (!sdLoggerBegin()) {
    Serial.println("ERROR: SD no inicializa");
    while (true) delay(1000);
  }
  sdLoggerCreateHeader(CSV_FILE);
  Serial.println("Sistema listo. Registro cada 30 segundos.");
}

void loop() {
  if (millis() - lastLog < LOG_INTERVAL_MS) return;
  lastLog = millis();

  SHT31Data sht;
  DateTimeData dt;
  ADS1115Data ads;
  INA219Data ina;
  NanoSDI12Data sdi12;

  sht31Read(sht);
  ds3231Read(dt);
  String ts = formatTimestamp(dt);

  Serial.println("\n========== REGISTRO ==========");
  Serial.println("Timestamp: " + ts);
  Serial.printf("Temp: %.2f C\n", sht.temperaturaC);
  Serial.printf("HR: %.2f %%\n", sht.humedadRH);
  Serial.println(sht.ok ? "SHT31 OK" : "SHT31 ERROR");
  Serial.println(dt.ok ? "RTC OK" : "RTC ERROR");

  bool ok = sdLoggerAppendFull(CSV_FILE, ts, sht, ads, ina, sdi12);
  Serial.println(ok ? "SD OK" : "SD ERROR");
}
