#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "oled_display.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool oledBegin() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) return false;
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("OLED OK");
  display.display();
  return true;
}

void oledShowINA219(const INA219Data &ina) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("INA219 Monitor");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  display.setCursor(0, 16); display.print("Vbus: "); display.print(ina.busVoltageV, 2); display.println(" V");
  display.setCursor(0, 28); display.print("I:    "); display.print(ina.currentmA, 1); display.println(" mA");
  display.setCursor(0, 40); display.print("P:    "); display.print(ina.powermW, 1); display.println(" mW");
  display.setCursor(0, 52); display.print("Vsh:  "); display.print(ina.shuntVoltagemV, 2); display.println(" mV");
  display.display();
}

void oledShowFull(const SHT31Data &sht, const INA219Data &ina, const ADS1115Data &ads) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Datalogger local");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  display.setCursor(0, 14); display.print("T: "); display.print(sht.temperaturaC, 1); display.print("C H: "); display.print(sht.humedadRH, 0); display.println("%");
  display.setCursor(0, 26); display.print("Vbus: "); display.print(ina.busVoltageV, 2); display.println("V");
  display.setCursor(0, 38); display.print("I: "); display.print(ina.currentmA, 1); display.println("mA");
  display.setCursor(0, 50); display.print("4-20: "); display.print(ads.corrienteA0_mA, 2); display.println("mA");
  display.display();
}

void oledShowError(const char* message) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ERROR");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  display.setCursor(0, 18);
  display.println(message);
  display.display();
}
