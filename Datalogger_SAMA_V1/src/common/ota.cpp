#include <Arduino.h>
#include <Ethernet.h>
#include <ESP_SSLClient.h>
#include <Update.h>
#include <MD5Builder.h>
#include <ArduinoJson.h>
#include "ota.h"
#include "config.h"

// ─────────────────────────────────────────────────────────────
// Utilidades internas
// ─────────────────────────────────────────────────────────────

static bool parseURL(const String& url, String& host, int& port, String& path) {
    String u = url;
    bool tls = false;

    if (u.startsWith("https://")) { u.remove(0, 8); tls = true; }
    else if (u.startsWith("http://")) { u.remove(0, 7); }
    else return false;

    port = tls ? 443 : 80;

    int colon = u.indexOf(':');
    int slash  = u.indexOf('/');
    if (colon > 0 && (slash < 0 || colon < slash)) {
        port = u.substring(colon + 1, slash < 0 ? (int)u.length() : slash).toInt();
        u.remove(colon, (slash < 0 ? (int)u.length() : slash) - colon);
        slash = u.indexOf('/');
    }

    host = (slash < 0) ? u : u.substring(0, slash);
    path = (slash < 0) ? "/" : u.substring(slash);
    return true;
}

// Compara versiones semánticas "X.Y.Z"
// Retorna  1 si a > b  |  0 si igual  | -1 si a < b
static int compareVersions(const String& a, const String& b) {
    int aMaj, aMin, aPat, bMaj, bMin, bPat;
    sscanf(a.c_str(), "%d.%d.%d", &aMaj, &aMin, &aPat);
    sscanf(b.c_str(), "%d.%d.%d", &bMaj, &bMin, &bPat);

    if (aMaj != bMaj) return aMaj > bMaj ? 1 : -1;
    if (aMin != bMin) return aMin > bMin ? 1 : -1;
    if (aPat != bPat) return aPat > bPat ? 1 : -1;
    return 0;
}

// ─────────────────────────────────────────────────────────────
// Conexión HTTPS genérica → devuelve body como String
// Maneja hasta 3 redirects automáticamente
// ─────────────────────────────────────────────────────────────

static String httpsGet(const String& startURL, int& outCode, int maxRedirects = 3) {
    String currentURL = startURL;
    outCode = 0;

    for (int rd = 0; rd <= maxRedirects; rd++) {
        String host, path;
        int port;
        if (!parseURL(currentURL, host, port, path)) return "";

        EthernetClient eth;
        ESP_SSLClient client;
        client.setClient(&eth);
        client.setInsecure();
        client.setBufferSizes(16384, 512);

        Serial.printf("[HTTP] %s%s\n", host.c_str(), path.c_str());

        if (!client.connect(host.c_str(), port)) {
            Serial.println("[HTTP] Error conexión");
            return "";
        }

        client.printf("GET %s HTTP/1.1\r\n", path.c_str());
        client.printf("Host: %s\r\n", host.c_str());
        client.print("User-Agent: ESP32-OTA/1.0\r\n");
        client.print("Connection: close\r\n\r\n");

        // Esperar respuesta
        unsigned long t = millis();
        while (!client.available() && millis() - t < 15000) delay(10);

        String statusLine = client.readStringUntil('\n');
        statusLine.trim();
        outCode = statusLine.substring(9, 12).toInt();

        String location = "";
        int contentLength = 0;

        while (client.connected() || client.available()) {
            String line = client.readStringUntil('\n');
            line.trim();
            if (line.length() == 0) break;
            if (line.startsWith("Location:"))       location = line.substring(9);
            if (line.startsWith("Content-Length:")) contentLength = line.substring(15).toInt();
        }

        if (outCode == 301 || outCode == 302) {
            location.trim();
            Serial.printf("[HTTP] Redirect → %s\n", location.c_str());
            currentURL = location;
            client.stop();
            eth.stop();
            continue;
        }

        if (outCode != 200) {
            Serial.printf("[HTTP] Error %d\n", outCode);
            return "";
        }

        // Leer body
        String body = "";
        unsigned long lastData = millis();
        while (client.connected() || client.available()) {
            if (client.available()) {
                body += (char)client.read();
                lastData = millis();
            } else {
                delay(1);
                if (millis() - lastData > 5000) break;
            }
        }
        return body;
    }
    return "";
}

// ─────────────────────────────────────────────────────────────
// ota_update — descarga y flashea firmware desde URL
// ─────────────────────────────────────────────────────────────

bool ota_update(const char* url, const char* expected_md5) {
    Serial.println("\n[OTA] Iniciando descarga...");

    String currentURL = String(url);

    for (int rd = 0; rd <= 3; rd++) {
        String host, path;
        int port;
        if (!parseURL(currentURL, host, port, path)) {
            Serial.println("[OTA] URL inválida"); return false;
        }

        Serial.printf("[OTA] Conectando a %s:%d\n", host.c_str(), port);

        EthernetClient eth;
        ESP_SSLClient client;
        client.setClient(&eth);
        client.setInsecure();
        client.setBufferSizes(16384, 512);

        if (!client.connect(host.c_str(), port)) {
            Serial.println("[OTA] Error conexión"); return false;
        }

        client.printf("GET %s HTTP/1.1\r\n", path.c_str());
        client.printf("Host: %s\r\n", host.c_str());
        client.print("User-Agent: ESP32-OTA/1.0\r\n");
        client.print("Connection: close\r\n\r\n");

        unsigned long t = millis();
        while (!client.available() && millis() - t < 15000) delay(10);

        String statusLine = client.readStringUntil('\n');
        statusLine.trim();
        Serial.println(statusLine);
        int code = statusLine.substring(9, 12).toInt();

        String location = "";
        int contentLength = 0;

        while (client.connected() || client.available()) {
            String line = client.readStringUntil('\n');
            line.trim();
            if (line.length() == 0) break;
            if (line.startsWith("Location:"))       location = line.substring(9);
            if (line.startsWith("Content-Length:")) contentLength = line.substring(15).toInt();
        }

        if (code == 301 || code == 302) {
            location.trim();
            Serial.printf("[OTA] Redirect → %s\n", location.c_str());
            currentURL = location;
            client.stop(); eth.stop();
            continue;
        }

        if (code != 200) { Serial.printf("[OTA] HTTP %d\n", code); return false; }

        if (contentLength <= 0) { Serial.println("[OTA] Tamaño inválido"); return false; }
        Serial.printf("[OTA] Tamaño: %d bytes\n", contentLength);

        if (!Update.begin(contentLength)) {
            Serial.print("[OTA] Sin espacio: "); Update.printError(Serial); return false;
        }

        MD5Builder md5;
        md5.begin();

        uint8_t buff[1024];
        int total = 0;
        unsigned long lastData = millis();

        while ((client.connected() || client.available()) && total < contentLength) {
            if (client.available()) {
                int toRead = min((int)sizeof(buff), contentLength - total);
                int len = client.read(buff, toRead);
                if (len > 0) {
                    size_t written = Update.write(buff, len);
                    if ((int)written != len) {
                        Serial.print("\n[OTA] Error flash: ");
                        Update.printError(Serial);
                        return false;
                    }
                    md5.add(buff, len);
                    total += len;
                    lastData = millis();
                    Serial.printf("[OTA] %d/%d (%.1f%%)\r", total, contentLength,
                                  100.0f * total / contentLength);
                }
            } else {
                delay(1);
                if (millis() - lastData > 10000) {
                    Serial.println("\n[OTA] Timeout"); break;
                }
            }
        }

        Serial.printf("\n[OTA] Bytes recibidos: %d\n", total);

        if (total != contentLength) {
            Serial.printf("[OTA] Incompleto %d/%d\n", total, contentLength);
            Update.abort(); return false;
        }

        md5.calculate();
        String calc = md5.toString();
        Serial.printf("[OTA] MD5: %s\n", calc.c_str());

        if (expected_md5 && strlen(expected_md5) > 0) {
            if (!calc.equalsIgnoreCase(expected_md5)) {
                Serial.println("[OTA] MD5 incorrecto");
                Update.abort(); return false;
            }
        }

        if (Update.end(true)) {
            Serial.println("[OTA] Éxito, reiniciando...");
            delay(1000);
            ESP.restart();
        } else {
            Serial.print("[OTA] Error al finalizar: ");
            Update.printError(Serial);
        }
        return true;
    }
    return false;
}

// ─────────────────────────────────────────────────────────────
// ota_check_and_update — lee version.json y actualiza si hay nueva versión
// ─────────────────────────────────────────────────────────────

bool ota_check_and_update() {
    Serial.println("[OTA] Verificando versión...");
    Serial.printf("[OTA] Versión actual: %s\n", FW_VERSION);

    int code;
    String body = httpsGet(VERSION_JSON_URL, code);

    if (body.isEmpty()) {
        Serial.println("[OTA] No se pudo obtener version.json");
        return false;
    }

    // Parsear JSON  { "version": "1.0.1", "url": "...", "md5": "" }
    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
        Serial.printf("[OTA] JSON inválido: %s\n", err.c_str());
        return false;
    }

    String remoteVersion = doc["version"] | "";
    String remoteURL     = doc["url"]     | "";
    String remoteMD5     = doc["md5"]     | "";

    Serial.printf("[OTA] Versión remota:  %s\n", remoteVersion.c_str());

    if (remoteVersion.isEmpty() || remoteURL.isEmpty()) {
        Serial.println("[OTA] version.json incompleto");
        return false;
    }

    if (compareVersions(remoteVersion, FW_VERSION) <= 0) {
        Serial.println("[OTA] Firmware al día, no se actualiza.");
        return false;
    }

    Serial.printf("[OTA] Nueva versión disponible: %s → %s\n", FW_VERSION, remoteVersion.c_str());
    return ota_update(remoteURL.c_str(), remoteMD5.c_str());
}
