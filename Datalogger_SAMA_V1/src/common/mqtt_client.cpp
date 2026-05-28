#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ESP_SSLClient.h>

#include "config.h"
#include "mqtt_client.h"

// =======================
// Clientes Ethernet / TLS / MQTT
// =======================
EthernetClient ethClient;
ESP_SSLClient sslClient;
PubSubClient mqttClient(sslClient);

// MAC para W5500
byte macW5500[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x10 };

// =======================
// Callback para OTA
// =======================
static void (*otaCallback)() = nullptr;

// =======================
// Callback MQTT
// =======================
static void onMqttMessage(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (unsigned int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }
    msg.trim();

    Serial.printf("[MQTT] Mensaje en '%s': %s\n", topic, msg.c_str());

    // Topic: sama/ota/update  →  payload esperado: "update"
    if (String(topic) == MQTT_TOPIC_OTA_UPDATE && msg == "update") {
        Serial.println("[MQTT] Comando OTA recibido");
        if (otaCallback) {
            otaCallback();
        }
    }
}

// =======================
// Reset W5500
// =======================
static void resetW5500() {
    pinMode(W5500_RST, OUTPUT);

    digitalWrite(W5500_RST, LOW);
    delay(200);

    digitalWrite(W5500_RST, HIGH);
    delay(500);
}

// =======================
// Reconectar MQTT
// =======================
static bool mqttReconnect() {
    if (mqttClient.connected()) {
        return true;
    }

    Serial.print("Conectando a broker MQTT TLS... ");

    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
        Serial.println("conectado");

        mqttClient.publish(MQTT_TOPIC_PUB, "ESP32 datalogger conectado por W5500 + MQTT TLS");
        mqttClient.subscribe(MQTT_TOPIC_OTA_UPDATE);

        return true;
    }

    Serial.print("fallo, rc=");
    Serial.println(mqttClient.state());

    return false;
}

// =======================
// Inicializar MQTT / Ethernet
// =======================
bool mqttBegin() {
    Serial.println("Inicializando W5500 + MQTT TLS...");

    // Asegurar CS en HIGH para evitar conflicto con SD
    pinMode(W5500_CS, OUTPUT);
    pinMode(SD_CS, OUTPUT);

    digitalWrite(W5500_CS, HIGH);
    digitalWrite(SD_CS, HIGH);

    resetW5500();

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, W5500_CS);

    Ethernet.init(W5500_CS);

    Serial.println("Solicitando IP por DHCP...");

    if (Ethernet.begin(macW5500) == 0) {
        Serial.println("Error: no se obtuvo IP por DHCP.");
        return false;
    }

    delay(1000);

    Serial.print("IP local: ");
    Serial.println(Ethernet.localIP());

    Serial.print("Gateway: ");
    Serial.println(Ethernet.gatewayIP());

    Serial.print("DNS: ");
    Serial.println(Ethernet.dnsServerIP());

    // TLS cifrado sin validación externa de certificado
    sslClient.setClient(&ethClient);
    sslClient.setInsecure();
    sslClient.setBufferSizes(4096, 1024);
    sslClient.setDebugLevel(1);

    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setBufferSize(512);
    mqttClient.setCallback(onMqttMessage);

    return mqttReconnect();
}

// =======================
// Mantener MQTT activo
// =======================
bool mqttLoop() {
    if (!mqttClient.connected()) {
        mqttReconnect();
    }

    mqttClient.loop();

    return mqttClient.connected();
}

bool mqttIsConnected() {
    return mqttClient.connected();
}

bool mqttPublish(const char* topic, const String& payload) {
    if (!mqttClient.connected()) {
        mqttReconnect();
    }

    if (!mqttClient.connected()) {
        return false;
    }

    return mqttClient.publish(topic, payload.c_str());
}

bool mqttPublishDefault(const String& payload) {
    return mqttPublish(MQTT_TOPIC_PUB, payload);
}

// =======================
// Establecer callback OTA
// =======================
void mqttSetOTACallback(void (*callback)()) {
    otaCallback = callback;
}
