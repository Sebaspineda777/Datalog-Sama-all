#pragma once
#include <Arduino.h>

// Descarga y flashea directamente desde una URL (con manejo de redirects)
bool ota_update(const char* url, const char* expected_md5 = "");

// Lee version.json, compara con FW_VERSION y actualiza solo si hay nueva versión
// Retorna true si actualizó, false si estaba al día o hubo error
bool ota_check_and_update();
