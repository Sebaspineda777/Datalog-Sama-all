#pragma once
#include <Arduino.h>
#ifndef CONFIG_H
#define CONFIG_H

// =======================
// FIRMWARE VERSION
// =======================
#define FW_VERSION  "1.0.3"

// =======================
// I2C
// =======================
#define I2C_SDA 21
#define I2C_SCL 22

// =======================
// SPI compartido W5500 + microSD
// =======================
#define SPI_SCK   18
#define SPI_MISO  19
#define SPI_MOSI  23

#define W5500_CS   5
#define W5500_RST  4

#define SD_CS      13

// =======================
// MQTT / EMQX
// =======================
#define MQTT_SERVER     "p9d17dd1.ala.us-east-1.emqxsl.com"
#define MQTT_PORT       8883

#define MQTT_USER       "Sama"
#define MQTT_PASS       "S4m4_mqtt*"
#define MQTT_CLIENT_ID  "Sama"

#define MQTT_TOPIC_PUB           "sama/test"
#define MQTT_TOPIC_OTA_UPDATE    "sama/ota/update"
#define MQTT_TOPIC_OTA_STATUS    "sama/ota/status"

// =======================
// OTA GitHub
// =======================
#define VERSION_JSON_URL  "https://github.com/Sebaspineda777/sama-OTA/raw/refs/heads/main/version.json"

// =======================
// Tiempos
// =======================
#define LOG_INTERVAL_MS 60000UL

#endif

// =======================
// I2C compartido
// =======================
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_CLOCK_HZ 100000

// Direcciones I2C detectadas
#define INA219_ADDR   0x40
#define SHT31_ADDR    0x44
#define ADS1115_ADDR  0x48
#define DS3231_ADDR   0x68
#define OLED_ADDR     0x3C

// =======================
// SPI microSD
// =======================
#define SPI_SCK   18
#define SPI_MISO  19
#define SPI_MOSI  23
#define SD_CS     13

// =======================
// UART2: receptor Arduino Nano / SDI-12 externo
// =======================
#define NANO_RX 16
#define NANO_TX 17
#define NANO_BAUD 9600

// =======================
// Archivo de registro
// =======================
#define CSV_FILE "/datalog.csv"
#define LOG_INTERVAL_MS 30000UL

// =======================
// ADS1115 / 4-20 mA
// =======================
#define R_SHUNT_OHMS 150.0f
