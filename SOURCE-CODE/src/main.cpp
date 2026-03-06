// Библиотеки
#include <Arduino.h>            // Базовые функции/фишки Arduino
#include <U8g2lib.h>            // Вывод на дисплей
#include <EEPROM.h>             // Энергонезависимая память
#include <Wire.h>               // Работа по шине I2C

// Файлы
#include "config.h"             // Настройки
#include "bitmap.h"             // Битмапы
#include "screen.hpp"           // Экран и интерфейс
#include "standby.hpp"          // Анимация ожидания
#include "battery.hpp"          // Баттарея
#include "button.hpp"           // Спам Wi-Fi сетями  
#include "packetmonitor.hpp"    // Монитор пакетов
#include "beacon.hpp"           // Спам точками доступа 
#include "filesystem.hpp"       // Файловая система 

// Функции
void setup() {                  // Инициализация
  Serial.begin(115200);         // Инициализация Serial
  Serial.println("ESP-HAVOC Device");
  Serial.println(FIRMWARE_VERSION);
  Serial.println(String("Build Date: ") + BUILD_DATE);
  Serial.println("Don`t be a skid");
  Serial.println("");

  // Serial.println("Initializing <Wire.h> (I2C Bus Library)...");
  // Wire.begin(OLED_SDA, OLED_SCL); // Инициализация шины I2C для работы дисплея

  Serial.println("Initializing 'battery.hpp' (Battery)...");
  setupBattery();               // Инициализация баттареи
  Serial.println("Initializing 'button.hpp' (Buttons)...");
  setupButtonPins();            // Инициализация пинов кнопок
  setupButtonTimers();          // Инициализация таймеров кнопок
  Serial.println("Initializing 'beaconspam.hpp' (Random SSID list for Beacon Spam)...");
  generateRandomSSIDList();     // Генерация рандомных SSID для спама

  Serial.println("Initializing 'filesystem.hpp' (LittleFS & EEPROM)...");
  LittleFS.begin(true);         // Инициализация LittleFS
  if (!LittleFS.exists(CONFIG_FILE_PATH)) { // Если первый запуск (то есть нету файла настроек)
    Serial.println("└─ Config file doesn`t exist (Creating new config)...");
    currentSettings = defaultSettings;  // Пишем стандартные настройки
    saveConfig(currentSettings); // Записываем в LittleFS
  } else {                      // Если не первый запуск
    Serial.println("└─ Config file exist (Loading config)...");
    loadConfig(currentSettings); // Читаем настройки
  }

  Serial.println("Initializing <U8g2lib.h> (Screen library)...");
  u8g2.begin();                  // Инициализация дисплея
  u8g2.setContrast(OLED_CONTRAST); // Установка контрасности/якроски дисплея

  setupScreenTimers();           // Ицициализация таймера логотипа при запуске

  while (millis() - lastSplashTime <= currentSettings.SPLASH_TIMEOUT && currentSettings.SPLASH_TIMEOUT) { // Если время не закончилось
    u8g2.clearBuffer();          // Очистка буфера
    u8g2.drawXBM(31, 27, 66, 11, bitmap_logo); // Отрисовка логотипа
    u8g2.sendBuffer();           // Отправка в буфер
  }
}
void loop() {
  mainMenu();                    // Главное меню
}
