#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "ads1115.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Etapa: ADS1115 4-20mA + analogicas ===");
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(I2C_CLOCK_HZ);

  if (!ads1115Begin()) {
    Serial.println("ERROR: ADS1115 no detectado en 0x48");
    while (true) delay(1000);
  }
  Serial.println("ADS1115 OK");
}

void loop() {
  ADS1115Data ads;
  ads1115ReadAll(ads);

  Serial.println("\n========== ADS1115 ==========");
  Serial.printf("A0 shunt: %.4f V\n", ads.voltajeA0);
  Serial.printf("A0 corriente: %.3f mA\n", ads.corrienteA0_mA);
  Serial.printf("A0 porcentaje: %.2f %%\n", ads.porcentaje4_20);
  Serial.printf("A1: %.4f V\n", ads.voltajeA1);
  Serial.printf("A2: %.4f V\n", ads.voltajeA2);
  Serial.printf("A3: %.4f V\n", ads.voltajeA3);
  delay(2000);
}
