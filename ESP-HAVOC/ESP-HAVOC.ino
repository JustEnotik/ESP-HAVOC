// Библиотеки
#include <Arduino.h>
#include <U8g2lib.h>
#include <EEPROM.h>
#include <Wire.h>

// Библиотеки ESP32
// #include <esp_wifi.h>
// #include <esp_netif.h>
// #include <esp_event.h>
// #include <esp_system.h>
// #include <esp_event_loop.h>
// // #include <esp_wifi_types.h>
// // #include <esp_private/wifi.h>

// Файлы
#include "bitmap.h"
#include "config.h"
#include "standby.h"
#include "battery.h"
#include "interface.h"
#include "packetmonitor.h"
#include "beaconspam.h"
#include "settings.h"

// Обьявление дисплея
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(OLED_ROTATION, OLED_RESET, OLED_SCL, OLED_SDA);

// Обьявление таймеров
unsigned long lastUpDebounceTime = 0;
unsigned long lastDownDebounceTime = 0;
unsigned long lastSelectDebounceTime = 0;
unsigned long lastBackDebounceTime = 0;
unsigned long lastFrameTime = 0; 
unsigned long lastBootLogoTime = 0; 
unsigned long lastActionTime = 0; 

// Обьявление функций
void listMenu(
  const char* const* items,
  const unsigned char* const* icons,
  uint8_t itemCount,
  // bool checkMarkSelectedIcon,
  void (*onSelect)(uint8_t selectedItem)) {
  lastUpDebounceTime = millis();
  lastDownDebounceTime = millis();
  lastSelectDebounceTime = millis();
  lastBackDebounceTime = millis();
  /* static */ uint8_t selectedItem = 0;

  while (true) {
    // Вверх
    if (digitalRead(BUTTON_UP) == LOW && (millis() - lastUpDebounceTime) > DEBOUNCE_DELAY) {
      selectedItem = constrain(selectedItem - 1, 0, itemCount - 1);
      lastUpDebounceTime = millis();
      lastActionTime = millis();
    }
    // Вниз
    if (digitalRead(BUTTON_DOWN) == LOW && (millis() - lastDownDebounceTime) > DEBOUNCE_DELAY) {
      selectedItem = constrain(selectedItem + 1, 0, itemCount - 1);
      lastDownDebounceTime = millis();
      lastActionTime = millis();
    }
    // Выбор
    if (digitalRead(BUTTON_SELECT) == LOW && (millis() - lastSelectDebounceTime) > DEBOUNCE_DELAY) {
      if (selectedItem == 0) {
        break;
      } else if (onSelect != nullptr) {
        onSelect(selectedItem);
      }
      lastSelectDebounceTime = millis();
      lastActionTime = millis();
    }
    // Назад
    if (digitalRead(BUTTON_BACK) == LOW && (millis() - lastBackDebounceTime) > DEBOUNCE_DELAY) {
      lastBackDebounceTime = millis();
      lastActionTime = millis();
      break;
    }

    u8g2.clearBuffer();
    bufferListMenu(u8g2, selectedItem, items, icons, itemCount);
    u8g2.sendBuffer();
  }
}

void packetMonitorMenu() {
  uint8_t selectedChannel = 1;
  setupPacketMonitor(selectedChannel);
  
  lastPacketMonitorTime = millis();
  lastUpDebounceTime = millis();
  lastDownDebounceTime = millis();
  lastSelectDebounceTime = millis();
  lastBackDebounceTime = millis();
  
  while (true) {
    handleBattery();

    // Вверх
    if (digitalRead(BUTTON_UP) == LOW && (millis() - lastUpDebounceTime) > DEBOUNCE_DELAY) {
      selectedChannel = constrain(selectedChannel - 1, 1, 14);
      esp_wifi_set_channel(selectedChannel, WIFI_SECOND_CHAN_NONE);
      lastUpDebounceTime = millis();
      lastActionTime = millis();
    }
    // Вниз
    if (digitalRead(BUTTON_DOWN) == LOW && (millis() - lastDownDebounceTime) > DEBOUNCE_DELAY) {
      selectedChannel = constrain(selectedChannel + 1, 1, 14);
      esp_wifi_set_channel(selectedChannel, WIFI_SECOND_CHAN_NONE);
      lastDownDebounceTime = millis();
      lastActionTime = millis();
    }
    // Назад
    if (digitalRead(BUTTON_BACK) == LOW) {
      teardownPacketMonitor();
      lastBackDebounceTime = millis();
      lastActionTime = millis();
      break;
    }

    // Обновление графика каждую секунду
    if (millis() - lastPacketMonitorTime >= 1000) {
      updateDetectedPacketsHistory();
      lastPacketMonitorTime = millis();  
    }
    // Обработка экрана 
    if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
      u8g2.clearBuffer();
      bufferPacketMonitorMenu(
      u8g2, 
      selectedChannel, 
      detectedPacketsTotal,
      detectedPacketsDeauth,
      detectedPacketsBeacon,
      detectedPacketsHistory);
      u8g2.sendBuffer();
    }
  }
}
void beaconSpamScreen(uint8_t selectedItem) {
  lastUpDebounceTime = millis();
  lastDownDebounceTime = millis();
  lastSelectDebounceTime = millis();
  lastBackDebounceTime = millis();

  setupBeaconSpam();

  while (true) {
    // Назад
    if (digitalRead(BUTTON_BACK) == LOW && (millis() - lastBackDebounceTime) > DEBOUNCE_DELAY) { 
      lastBackDebounceTime = millis();
      lastActionTime = millis();
      // teardownBeaconSpam(); 
      break;
    }

    switch (selectedItem) {
      case 1: handleBeaconSpam(rickrollSSID, sizeof(rickrollSSID) / sizeof(rickrollSSID[0])); break;
      case 2: { const char* tempSSID[] = {randomSSID()}; handleBeaconSpam(tempSSID, 1); break; }
      case 3: handleBeaconSpam(funnySSID, sizeof(funnySSID) / sizeof(funnySSID[0])); break;
    }

    u8g2.clearBuffer();
    bufferBeaconSpamScreen(
    u8g2, 
    BEACON_SPAM_MENU_ITEMS[selectedItem]);
    u8g2.sendBuffer();
  }
}

// Инициализация
void setup() {
  // Инициализация шины I2C для работы дисплея
  Wire.begin(OLED_SDA, OLED_SCL);

  // Инициализация пинов
  pinMode(VOLTAGE_PIN, INPUT);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  pinMode(BUTTON_BACK, INPUT_PULLUP);

  // Инициализация EEPROM
  EEPROM.begin(512);

  // Инициализация настроек
  if (EEPROM.read(0) != SETTINGS_VERSION) { // Если первый запуск
    EEPROM.put(1, settings);              // Пишем стандартные настройки
    EEPROM.write(0, SETTINGS_VERSION);      // Делаем первый запуск не актуальным
    EEPROM.commit();                      // Записываем в EEPROM
  } else {                                // Если не первый запуск
    EEPROM.get(1, settings);              // Читаем настройки
  }

  // Инициализация дисплея
  u8g2.begin();
  u8g2.setContrast(OLED_CONTRAST);

    // Ицициализация таймеров
  lastFrameTime = millis(); 
  lastBootLogoTime = millis();
  lastActionTime = millis();
}
// Цикл
void loop() {
  handleBattery();
  static uint8_t selectedItem = 0;

  // Вывод экрана с логотипом
  while (millis() - lastBootLogoTime <= settings.BOOT_LOGO_DELAY && settings.BOOT_LOGO_DELAY) {
    u8g2.clearBuffer();
    u8g2.drawXBM(31, 27, 66, 11, bitmap_boot_logo);
    u8g2.sendBuffer();
  }

  // Вверх
  if (digitalRead(BUTTON_UP) == LOW && (millis() - lastUpDebounceTime) > DEBOUNCE_DELAY) {
    if (millis() - lastActionTime < STANDBY_TIMEOUT) {
      selectedItem = constrain(selectedItem - 1, 0, 4);
    }
    lastUpDebounceTime = millis();
    lastActionTime = millis();
  }
  // Вниз
  if (digitalRead(BUTTON_DOWN) == LOW && (millis() - lastDownDebounceTime) > DEBOUNCE_DELAY) {
    if (millis() - lastActionTime < STANDBY_TIMEOUT) {
      selectedItem = constrain(selectedItem + 1, 0, 4);
    }
    lastDownDebounceTime = millis();
    lastActionTime = millis();
  }
  // Выбор
  if (digitalRead(BUTTON_SELECT) == LOW && (millis() - lastSelectDebounceTime) > DEBOUNCE_DELAY) {
    if (millis() - lastActionTime < STANDBY_TIMEOUT) {
      switch (selectedItem) {
        case 0:
        listMenu(
          WIFI_MENU_ITEMS, 
          WIFI_MENU_ICONS, 
          sizeof(WIFI_MENU_ITEMS) / sizeof(WIFI_MENU_ITEMS[0]), 
          [](uint8_t wifi_selected) {
            switch (wifi_selected) {
              // case 0; break;
              case 1: packetMonitorMenu(); break;
              case 2: 
              listMenu(
                BEACON_SPAM_MENU_ITEMS,
                BEACON_SPAM_MENU_ICONS,
                sizeof(BEACON_SPAM_MENU_ITEMS) / sizeof(BEACON_SPAM_MENU_ITEMS[0]), 
                [](uint8_t beacon_spam_selected) {
                  switch (beacon_spam_selected) {
                    // case 0; break;
                    case 1: beaconSpamScreen(beacon_spam_selected); break;
                    case 2: beaconSpamScreen(beacon_spam_selected); break;
                    case 3: beaconSpamScreen(beacon_spam_selected); break;
                  }
                }
              ); break;
              case 3: break;
            }
          }
        ); break;
        case 1: break;
        case 2: break;
        case 3: break;
        case 4:         
        listMenu(
          SETTINGS_MENU_ITEMS, 
          SETTINGS_MENU_ICONS, 
          sizeof(SETTINGS_MENU_ITEMS) / sizeof(SETTINGS_MENU_ITEMS[0]), 
          [](uint8_t settingsSelected) {
            switch (settingsSelected) {
              // case 0; break;
              case 1: 
              listMenu(
                INTERFACE_SETTINGS_MENU_ITEMS, 
                INTERFACE_SETTINGS_MENU_ICONS, 
                sizeof(INTERFACE_SETTINGS_MENU_ITEMS) / sizeof(INTERFACE_SETTINGS_MENU_ITEMS[0]), 
                [](uint8_t interfaceSettingsSelected) {
                  switch (interfaceSettingsSelected) {
                    // case 0; break;
                    case 1: listMenu(
                      INTERFACE_MAIN_MENU_TYPE_SETTINGS_MENU_ITEMS, 
                      INTERFACE_MAIN_MENU_TYPE_SETTINGS_MENU_ICONS, 
                      sizeof(INTERFACE_MAIN_MENU_TYPE_SETTINGS_MENU_ITEMS) / sizeof(INTERFACE_MAIN_MENU_TYPE_SETTINGS_MENU_ITEMS[0]), 
                      [](uint8_t interfaceMainMenuTypeSelected) {
                        switch (interfaceMainMenuTypeSelected) {
                          // case 0; break;
                          case 1: settings.MAIN_MENU_TYPE = 1; EEPROM.put(1, settings); EEPROM.commit(); break;
                          case 2: settings.MAIN_MENU_TYPE = 2; EEPROM.put(1, settings); EEPROM.commit(); break;
                          // u8g2.setRotation(OLED_ROTATION);
                        }
                      }
                    ); break;
                    case 2: listMenu(
                      INTERFACE_STANDBY_TIMEOUT_SETTINGS_MENU_ITEMS, 
                      INTERFACE_STANDBY_TIMEOUT_SETTINGS_MENU_ICONS, 
                      sizeof(INTERFACE_STANDBY_TIMEOUT_SETTINGS_MENU_ITEMS) / sizeof(INTERFACE_STANDBY_TIMEOUT_SETTINGS_MENU_ITEMS[0]), 
                      [](uint8_t interfaceMainMenuTypeSelected) {
                        switch (interfaceMainMenuTypeSelected) {
                          // case 0; break;
                          case 1: settings.STANDBY_TIMEOUT = 0; EEPROM.put(1, settings); EEPROM.commit(); break;
                          case 2: settings.STANDBY_TIMEOUT = 30; EEPROM.put(1, settings); EEPROM.commit(); break;
                          case 3: settings.STANDBY_TIMEOUT = 1 * 60; EEPROM.put(1, settings); EEPROM.commit(); break;
                          case 4: settings.STANDBY_TIMEOUT = 5 * 60; EEPROM.put(1, settings); EEPROM.commit(); break;
                          case 5: settings.STANDBY_TIMEOUT = 15 * 60; EEPROM.put(1, settings); EEPROM.commit(); break;
                          case 6: settings.STANDBY_TIMEOUT = 30 * 60; EEPROM.put(1, settings); EEPROM.commit(); break;
                        }
                      }
                    ); break;
                    case 3: listMenu(
                      INTERFACE_BOOT_LOGO_DELAY_SETTINGS_MENU_ITEMS, 
                      INTERFACE_BOOT_LOGO_DELAY_SETTINGS_MENU_ICONS, 
                      sizeof(INTERFACE_BOOT_LOGO_DELAY_SETTINGS_MENU_ITEMS) / sizeof(INTERFACE_BOOT_LOGO_DELAY_SETTINGS_MENU_ITEMS[0]), 
                      [](uint8_t interfaceMainMenuTypeSelected) {
                        switch (interfaceMainMenuTypeSelected) {
                          // case 0; break;
                          case 1: settings.BOOT_LOGO_DELAY = 0; EEPROM.put(1, settings); EEPROM.commit(); break;
                          case 2: settings.BOOT_LOGO_DELAY = 250; EEPROM.put(1, settings); EEPROM.commit(); break;
                          case 3: settings.BOOT_LOGO_DELAY = 500; EEPROM.put(1, settings); EEPROM.commit(); break;
                          case 4: settings.BOOT_LOGO_DELAY = 1 * 1000; EEPROM.put(1, settings); EEPROM.commit(); break;
                          case 5: settings.BOOT_LOGO_DELAY = 2 * 1000; EEPROM.put(1, settings); EEPROM.commit(); break;
                          case 6: settings.BOOT_LOGO_DELAY = 5 * 1000; EEPROM.put(1, settings); EEPROM.commit(); break;
                        }
                      }
                    ); break;
                  }
                }
              ); break;
              case 2: break;
              case 3: break;
            }
          }
        ); break;
      }
    }
    lastSelectDebounceTime = millis();
    lastActionTime = millis();
  }

  // Обработка экрана 
  if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
    if (millis() - lastActionTime < STANDBY_TIMEOUT || !STANDBY_TIMEOUT) {
      u8g2.clearBuffer();
      if (settings.MAIN_MENU_TYPE == 1) {
        bufferMainMenu(u8g2, MAIN_MENU_ITEMS, MAIN_MENU_ICONS, sizeof(MAIN_MENU_ITEMS) / sizeof(MAIN_MENU_ITEMS[0]), selectedItem);
      } else if (settings.MAIN_MENU_TYPE == 2) {
        bufferStationMainMenu(u8g2, MAIN_MENU_ITEMS, MAIN_MENU_ICONS, sizeof(MAIN_MENU_ITEMS) / sizeof(MAIN_MENU_ITEMS[0]), selectedItem);
      }
      bufferBattery(u8g2, batteryVoltage, batteryPercent);
      u8g2.sendBuffer();
      lastFrameTime = millis();
    } else {
      drawStandbyAnimation(u8g2);
    }
  }
}
