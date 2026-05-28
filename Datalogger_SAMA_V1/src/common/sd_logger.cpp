#include <SPI.h>
#include <SD.h>
#include "config.h"
#include "sd_logger.h"

bool sdLoggerBegin() {
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  if (!SD.begin(SD_CS, SPI)) {
    return false;
  }
  return SD.cardType() != CARD_NONE;
}

bool sdLoggerCreateHeader(const char* path) {
  if (SD.exists(path)) return true;

  File file = SD.open(path, FILE_WRITE);
  if (!file) return false;

  file.println("timestamp,temp_C,humedad_RH,sht31_ok,ads_a0_V,ads_a0_mA,ads_a0_pct,ads_a1_V,ads_a2_V,ads_a3_V,ads_ok,ina_vbus_V,ina_current_mA,ina_power_mW,ina_ok,sdi12_wp_kPa,sdi12_temp_C,sdi12_ok,sdi12_raw");
  file.close();
  return true;
}

bool sdLoggerAppendFull(const char* path, const String &timestamp,
                        const SHT31Data &sht, const ADS1115Data &ads,
                        const INA219Data &ina, const NanoSDI12Data &sdi12) {
  File file = SD.open(path, FILE_APPEND);
  if (!file) return false;

  file.print(timestamp); file.print(',');
  file.print(sht.temperaturaC, 2); file.print(',');
  file.print(sht.humedadRH, 2); file.print(',');
  file.print(sht.ok ? "true" : "false"); file.print(',');

  file.print(ads.voltajeA0, 4); file.print(',');
  file.print(ads.corrienteA0_mA, 3); file.print(',');
  file.print(ads.porcentaje4_20, 2); file.print(',');
  file.print(ads.voltajeA1, 4); file.print(',');
  file.print(ads.voltajeA2, 4); file.print(',');
  file.print(ads.voltajeA3, 4); file.print(',');
  file.print(ads.ok ? "true" : "false"); file.print(',');

  file.print(ina.busVoltageV, 3); file.print(',');
  file.print(ina.currentmA, 2); file.print(',');
  file.print(ina.powermW, 2); file.print(',');
  file.print(ina.ok ? "true" : "false"); file.print(',');

  file.print(sdi12.waterPotentialKPa, 2); file.print(',');
  file.print(sdi12.temperaturaC, 2); file.print(',');
  file.print(sdi12.jsonOk ? "true" : "false"); file.print(',');

  String rawEscaped = sdi12.raw;
  rawEscaped.replace("\"", "'");
  file.print('"'); file.print(rawEscaped); file.println('"');

  file.close();
  return true;
}
