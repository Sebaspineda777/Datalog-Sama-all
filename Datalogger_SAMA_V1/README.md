# Datalogger SAMA - Proyecto modular PlatformIO

Proyecto organizado por etapas para ESP32 DevKitC V4.

## Etapas disponibles

- `stage_sht31_rtc_sd`: SHT31 + DS3231 + microSD en CSV cada 30 s.
- `stage_ads1115`: ADS1115, entrada A0 para 4-20 mA con shunt de 150 ohm, A1-A3 analogicas.
- `stage_ina219_oled`: INA219 mostrado en OLED SSD1306 0.96".
- `stage_sdi12_nano`: ESP32 recibe JSON desde Arduino Nano por UART2 para datos SDI-12.
- `full_local`: integra SHT31 + DS3231 + SD + ADS1115 + INA219 + OLED + recepcion Nano/SDI-12, sin MQTT.

## Compilar una etapa

En PlatformIO puedes seleccionar el environment o usar terminal:

```bash
pio run -e stage_sht31_rtc_sd
pio run -e stage_ads1115
pio run -e stage_ina219_oled
pio run -e stage_sdi12_nano
pio run -e full_local
```

Subir:

```bash
pio run -e full_local -t upload
```

Monitor:

```bash
pio device monitor -b 115200
```

## Pines principales

I2C:
- SDA: GPIO21
- SCL: GPIO22

SPI microSD:
- SCK: GPIO18
- MISO: GPIO19
- MOSI: GPIO23
- CS SD: GPIO13

UART2 Nano/SDI-12 externo:
- RX: GPIO16
- TX: GPIO17

## Direcciones I2C

- INA219: 0x40
- SHT31: 0x44
- ADS1115: 0x48
- EEPROM RTC: 0x57
- DS3231: 0x68
- OLED SSD1306: 0x3C

## Archivo CSV

El entorno `full_local` genera `/datalog.csv` con:

```csv
timestamp,temp_C,humedad_RH,sht31_ok,ads_a0_V,ads_a0_mA,ads_a0_pct,ads_a1_V,ads_a2_V,ads_a3_V,ads_ok,ina_vbus_V,ina_current_mA,ina_power_mW,ina_ok,sdi12_wp_kPa,sdi12_temp_C,sdi12_ok,sdi12_raw
```

## Próximo paso sugerido

Cuando `full_local` quede estable, se puede crear una nueva etapa `full_mqtt` integrando W5500 + EMQX/HiveMQ.
