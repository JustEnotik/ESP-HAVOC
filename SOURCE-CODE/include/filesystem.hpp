// Что бы не было ошибок
#pragma once                    // В душе не чаю

// Библиотеки
#include <Arduino.h>            // Базовые функции/фишки Arduino
#include <U8g2lib.h>            // Вывод на дисплей
#include <LittleFS.h>           // Файловая системя для энергонезависимой памяти (EEPROM)

// Файлы
#include "config.h"             // Настройки

// Функции
bool saveConfig(const settings& cfg) {
  if (!LittleFS.exists("/.esphavoc")) {
    if (!LittleFS.mkdir("/.esphavoc")) {
      return false;
    }
  }
  File file = LittleFS.open(CONFIG_FILE_PATH, "w");
  if (!file) {
    return false;
  }
  bool success = file.write((uint8_t*)&cfg, sizeof(cfg)) == sizeof(cfg);
  file.close();
  return success;
}

bool loadConfig(settings& cfg) {
  if (!LittleFS.exists(CONFIG_FILE_PATH)) {
    return false;
  }
  File file = LittleFS.open(CONFIG_FILE_PATH, "r");
  if (!file) {
    return false;
  } 
  bool success = file.read((uint8_t*)&cfg, sizeof(cfg)) == sizeof(cfg);
  file.close();
  return success;
}
