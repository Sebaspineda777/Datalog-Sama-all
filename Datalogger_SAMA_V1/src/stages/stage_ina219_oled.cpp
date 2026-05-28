#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "ina219.h"
#include "oled_display.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Etapa: INA219 + OLED SSD1306 ===");
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(I2C_CLOCK_HZ);

  if (!oledBegin()) {
    Serial.println("ERROR: OLED no detectada en 0x3C");
    while (true) delay(1000);
  }

  if (!ina219Begin()) {
    Serial.println("ERROR: INA219 no detectado en 0x40");
    oledShowError("INA219 no detectado");
    while (true) delay(1000);
  }
  Serial.println("INA219 + OLED OK");
}

void loop() {
  INA219Data ina;
  ina219Read(ina);
  Serial.println("\n========== INA219 ==========");
  Serial.printf("Vbus: %.3f V\n", ina.busVoltageV);
  Serial.printf("Vshunt: %.3f mV\n", ina.shuntVoltagemV);
  Serial.printf("Corriente: %.2f mA\n", ina.currentmA);
  Serial.printf("Potencia: %.2f mW\n", ina.powermW);
  oledShowINA219(ina);
  delay(2000);
}
