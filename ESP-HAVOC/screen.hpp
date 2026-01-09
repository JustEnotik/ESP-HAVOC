// Что бы не было ошибок
#pragma once                    // В душе не чаю

// Библиотеки
#include <Arduino.h>            // Базовые функции/фишки Arduino
#include <U8g2lib.h>            // Вывод на дисплей

// Файлы
#include "config.h"             // Настройки
#include "bitmap.h"             // Битмапы
#include "button.hpp"           // Кнопки
#include "beacon.hpp"           // Спам точками доступа 
#include "deauth.hpp"           // Спам пакетами дезаунтедификации 
#include "standby.hpp"          // Анимация ожидания
#include "battery.hpp"          // Батарея
#include "filesystem.hpp"       // Файловая система 
#include "packetmonitor.hpp"    // Монитор пакетов

// Обьявление дисплея
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(UI_ROTATION, OLED_RESET, OLED_SCL, OLED_SDA);

// Переменные
unsigned long lastFrameTime = 0; // Таймер кадра (что бь не вылезать за максимальный FPS)
unsigned long lastSplashTime = 0; // Таймер логотипа при запуске
unsigned long lastPacketMonitorTime = 0; // Таймер монитора пакетов

// Функции
void setupScreenTimers() {      // Инициализация таймеров
  lastFrameTime = millis();     // Таймер кадра (что бь не вылезать за максимальный FPS)
  lastSplashTime = millis();      // Таймер логотипа при запуске
}

void drawListMenu (             // Отрисовка спискового меню
  U8G2& u8g2,                   // Библиотека вывода на дисплей
  const char* const* items,     // Список пунктов
  const uint8_t* const* icons,  // Список иконок
  uint8_t itemCount,            // Количесво пунктов
  int selectedItem) {           // Выбранный пункт
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setDrawColor(1);
  u8g2.setFont(UI_FONT);

  int currentPage = selectedItem / 4; // Текущая страница

  for (int i = 0; i + currentPage * 4 < itemCount; i++) { // Цикл отрисовки пунктов
    u8g2.drawXBM(4, 4 + i * 16, 8, 8, icons[i + currentPage * 4]); // Иконка
    u8g2.drawStr(16, 12 + i * 16, items[i + currentPage * 4]); // Текст
    // u8g2.drawStr(124 - (strlen(items[i])) * 6 , 12 + i * 16, items[i + currentPage * 4]); // Пункты
  }

  u8g2.setDrawColor(2);
  u8g2.drawBox(0, (selectedItem % 4) * 16, 128, 16); // Обводка выбранного пункта
}
void drawClassicMainMenu(      // Отрисовка Классического Main Menu
  U8G2& u8g2, 
  const char* const* items,
  const unsigned char** icons,
  uint8_t itemCount,
  int selectedItem) {
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(UI_FONT);

  // Иконка 1
  u8g2.drawXBM(16, 21, 16, 16, icons[0]);
  // Иконка 2
  u8g2.drawXBM(36, 21, 16, 16, icons[1]);
  // Иконка 3
  u8g2.drawXBM(56, 21, 16, 16, icons[2]);
  // Иконка 4
  u8g2.drawXBM(96, 21, 16, 16, icons[4]);
  // Иконка 5
  u8g2.drawXBM(76, 21, 16, 16, icons[3]);

  // Название выбранного пункта
  u8g2.drawStr((124 - (strlen(items[selectedItem]) * 6)) / 2, 48, items[selectedItem]);
  // Рамка выбранного пункта
  u8g2.setDrawColor(2);
  u8g2.drawBox(selectedItem * 20 + 15, 20, 18, 18);
}
void drawStationMainMenu(      // Отрисовка Station Main Menu
  U8G2& u8g2, 
  const char* const* items,
  const unsigned char** icons,
  uint8_t itemCount,
  int selectedItem) {

  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);

  // Обводки
  /* u8g2.drawFrame(45, 19, 22, 22);
  u8g2.drawFrame(69, 19, 22, 22);
  u8g2.drawFrame(93, 19, 22, 22);
  u8g2.drawFrame(117, 19, 22, 22); */

  // Иконки
  /* u8g2.drawXBM(0, 22, 16, 16, icons[-1 + selectedItem]);
  u8g2.drawXBM(24, 22, 16, 16, icons[0 + selectedItem]);
  u8g2.drawXBM(48, 22, 16, 16, icons[1 + selectedItem]);
  u8g2.drawXBM(72, 22, 16, 16, icons[2 + selectedItem]);
  u8g2.drawXBM(120, 22, 16, 16, icons[3 + selectedItem]);
  u8g2.drawXBM(96, 22, 16, 16, icons[4 + selectedItem]); */
  u8g2.drawXBM(28, 41, 8, 8, bitmap_small_arrows);

  int indices[] = {-1, 0, 1, 2, 4, 3};
  int positions[] = {0, 24, 48, 72, 120, 96};
  
  // Рисуем иконки с проверкой границ
  for (int i = 0; i < 6; i++) {
    int iconIndex = selectedItem + indices[i];
    
    // Проверяем что индекс в пределах массива
    if (iconIndex >= 0 && iconIndex < itemCount && icons[iconIndex] != NULL) {
      u8g2.drawXBM(positions[i], 22, 16, 16, icons[iconIndex]);
    }
  }

  // Текст выбранного пункта
  u8g2.setFont(UI_FONT);
  u8g2.drawStr(47, 49, items[selectedItem]);

  // Выделение выбранного пункта
  u8g2.setDrawColor(2);
  u8g2.drawBox(21, 19, 22, 32);
}
void drawAboutScreen(          // Отрисовка экрана About
  String FIRMWARE_VERSION,      // Версия прошивки
  String BUILD_DATE) {          // Дата билда
  u8g2.setFontMode(1);          // Установка режима шрифта
  u8g2.setBitmapMode(1);        // Установка режима битмапа
  u8g2.drawXBM(4, 49, 66, 11, bitmap_logo); // Отрисовка логотипа

  u8g2.setFont(UI_FONT);        // Установка шрифта
  u8g2.drawStr(4, 11, (FIRMWARE_VERSION).c_str()); // Отрисовка версии
  u8g2.drawStr(4, 20, ("Build Date: " + BUILD_DATE).c_str()); // Отрисовка даты билда
  u8g2.drawStr(4, 30, "Don't be a skid"); // Отрисовка дисклеймера
}

void wifiMenu() {
  while (!buttonBackClicked) {
    handleButtons();

    if (buttonUpClicked) { // Проверка на нажатие кнопки Up
      menuPointers.WIFI = constrain(menuPointers.WIFI - 1, 0, menuItemCount.WIFI - 1); // Отнимаем у выбранного пункта 1 с лимитом
    }
    if (buttonDownClicked) { // Проверка на нажатие кнопки Down
      menuPointers.WIFI = constrain(menuPointers.WIFI + 1, 0, menuItemCount.WIFI - 1); // Добавляем к выбранному пункту 1 с лимитом
    }
    if (buttonSelectClicked) { // Проверка на нажатие кнопки Select
      switch(menuPointers.WIFI) {
        case 0: buttonBackClicked = true;  break;
        case 1: startDeauthFlood(1); while (!buttonBackClicked) {
          handleButtons();

          if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
            u8g2.clearBuffer();
            drawDeauthFloodScreen(u8g2, "Deauth Flood", currentDeauthSSID);
            u8g2.sendBuffer();
          }
        } stopDeauthFlood(); yield(); buttonBackClicked = false; 
        break;
        case 2: while (!buttonBackClicked) {
          handleButtons();

          if (buttonUpClicked) { // Проверка на нажатие кнопки Up
            menuPointers.BEACON_FLOOD = constrain(menuPointers.BEACON_FLOOD - 1, 0, menuItemCount.BEACON_FLOOD - 1); // Отнимаем у выбранного пункта 1 с лимитом
          }
          if (buttonDownClicked) { // Проверка на нажатие кнопки Down
            menuPointers.BEACON_FLOOD = constrain(menuPointers.BEACON_FLOOD + 1, 0, menuItemCount.BEACON_FLOOD - 1); // Добавляем к выбранному пункту 1 с лимитом
          }
          if (buttonSelectClicked) {
            switch(menuPointers.BEACON_FLOOD) {
              case 0: buttonBackClicked = true; break;
              case 1: setupBeaconFlood(); while (!buttonBackClicked) {
                handleBeaconFlood(funnySSID, funnySSIDCount); 
                handleButtons();
                if (buttonUpClicked) { // Проверка на нажатие кнопки Up
                  beaconFloodChannel = constrain(beaconFloodChannel - 1, 1, 14); // Отнимаем у выбранного пункта 1 с лимитом
                }
                if (buttonDownClicked) { // Проверка на нажатие кнопки Down
                  beaconFloodChannel = constrain(beaconFloodChannel + 1, 1, 14); // Добавляем к выбранному пункту 1 с лимитом
                }

                // Обработка экрана
                if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
                  u8g2.clearBuffer();
                  drawBeaconFloodScreen(u8g2, "Funny Flood", funnySSID[currentBeaconIndexSSID]);
                  u8g2.sendBuffer();
                } 
              } teardownBeaconFlood(); buttonBackClicked = false; break;
              case 2: setupBeaconFlood(); while (!buttonBackClicked) {
                handleBeaconFlood(rickySSID, rickySSIDCount); 
                handleButtons();
                if (buttonUpClicked) { // Проверка на нажатие кнопки Up
                  beaconFloodChannel = constrain(beaconFloodChannel - 1, 1, 14); // Отнимаем у выбранного пункта 1 с лимитом
                }
                if (buttonDownClicked) { // Проверка на нажатие кнопки Down
                  beaconFloodChannel = constrain(beaconFloodChannel + 1, 1, 14); // Добавляем к выбранному пункту 1 с лимитом
                }

                // Обработка экрана
                if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
                  u8g2.clearBuffer();
                  drawBeaconFloodScreen(u8g2, "Ricky Flood", rickySSID[currentBeaconIndexSSID]);
                  u8g2.sendBuffer();
                }
              } teardownBeaconFlood(); buttonBackClicked = false; break;
              case 3: setupBeaconFlood(); while (!buttonBackClicked) {
                handleBeaconFlood((const char**)randomSSID, randomSSIDCount); 
                handleButtons();
                if (buttonUpClicked) { // Проверка на нажатие кнопки Up
                  beaconFloodChannel = constrain(beaconFloodChannel - 1, 1, 14); // Отнимаем у выбранного пункта 1 с лимитом
                }
                if (buttonDownClicked) { // Проверка на нажатие кнопки Down
                  beaconFloodChannel = constrain(beaconFloodChannel + 1, 1, 14); // Добавляем к выбранному пункту 1 с лимитом
                }

                // Обработка экрана
                if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
                  u8g2.clearBuffer();
                  drawBeaconFloodScreen(u8g2, "Random Flood", randomSSID[currentBeaconIndexSSID]);
                  u8g2.sendBuffer();
                }
              } teardownBeaconFlood(); buttonBackClicked = false; break;
            }
          }

          // Обработка экрана
          if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
            u8g2.clearBuffer();
            drawListMenu(u8g2, menuItemNames.BEACON_FLOOD, menuIconNames.BEACON_FLOOD, menuItemCount.BEACON_FLOOD, menuPointers.BEACON_FLOOD);
            u8g2.sendBuffer();
          }
        } buttonBackClicked = false; break;
        case 3: setupPacketMonitor(); while (!buttonBackClicked) {
          handleButtons();

          if (buttonUpClicked) { // Проверка на нажатие кнопки Up
            packetMonitorChannel = constrain(packetMonitorChannel - 1, 1, 14); // Отнимаем у выбранного пункта 1 с лимитом
          }
          if (buttonDownClicked) { // Проверка на нажатие кнопки Down
            packetMonitorChannel = constrain(packetMonitorChannel + 1, 1, 14); // Добавляем к выбранному пункту 1 с лимитом
          }

          // Обработка экрана и монитора пакетов
          if (millis() - lastPacketMonitorTime >= 1000) {
            handlePacketMonitor();
            lastPacketMonitorTime = millis();
          }
          if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
            u8g2.clearBuffer();
            drawPacketMonitorMenu(u8g2);
            u8g2.sendBuffer();
          }
        } teardownPacketMonitor(); buttonBackClicked = false; break;
      }
    }

    // Обработка экрана
    if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
      u8g2.clearBuffer();
      drawListMenu(u8g2, menuItemNames.WIFI, menuIconNames.WIFI, menuItemCount.WIFI, menuPointers.WIFI);
      u8g2.sendBuffer();
    }
  } buttonBackClicked = false; 
}
void aboutScreen() {
  while (!buttonBackClicked) {
    handleButtons();

    // Обработка экрана
    if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
      u8g2.clearBuffer();
      drawAboutScreen(FIRMWARE_VERSION, BUILD_DATE);
      u8g2.sendBuffer();
    }
  }
}
void settingsScreen() {
  while (!buttonBackClicked) {
    handleButtons();

    if (buttonSelectClicked) {
      switch(menuPointers.SETTINGS) {
        case 0: buttonBackClicked = true; break;
        case 1: while (!buttonBackClicked) {
          handleButtons();

          if (buttonUpClicked) { // Проверка на нажатие кнопки Up
            menuPointers.MAIN_MENU_TYPE = constrain(menuPointers.MAIN_MENU_TYPE - 1, 0, menuItemCount.MAIN_MENU_TYPE - 1); // Отнимаем у выбранного пункта 1 с лимитом
          }
          if (buttonDownClicked) { // Проверка на нажатие кнопки Down
            menuPointers.MAIN_MENU_TYPE = constrain(menuPointers.MAIN_MENU_TYPE + 1, 0, menuItemCount.MAIN_MENU_TYPE - 1); // Добавляем к выбранному пункту 1 с лимитом
          }
          if (buttonSelectClicked) { // Проверка на нажатие кнопки Select
            switch(menuPointers.MAIN_MENU_TYPE) {
              case 0: buttonBackClicked = true; break;
              case 1: currentSettings.MAIN_MENU_TYPE = 1; saveConfig(currentSettings); buttonBackClicked = true; break;
              case 2: currentSettings.MAIN_MENU_TYPE = 2; saveConfig(currentSettings); buttonBackClicked = true; break;
            }
          }

          // Обработка экрана
          if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
            u8g2.clearBuffer();
            drawListMenu(u8g2, menuItemNames.MAIN_MENU_TYPE, menuIconNames.MAIN_MENU_TYPE, menuItemCount.MAIN_MENU_TYPE, menuPointers.MAIN_MENU_TYPE);
            u8g2.sendBuffer();
          }
        } buttonBackClicked = false; break;
        case 2: while (!buttonBackClicked) {
          handleButtons();

          if (buttonUpClicked) { // Проверка на нажатие кнопки Up
            menuPointers.BATTERY_ICON_TYPE = constrain(menuPointers.BATTERY_ICON_TYPE - 1, 0, menuItemCount.BATTERY_ICON_TYPE - 1); // Отнимаем у выбранного пункта 1 с лимитом
          }
          if (buttonDownClicked) { // Проверка на нажатие кнопки Down
            menuPointers.BATTERY_ICON_TYPE = constrain(menuPointers.BATTERY_ICON_TYPE + 1, 0, menuItemCount.BATTERY_ICON_TYPE - 1); // Добавляем к выбранному пункту 1 с лимитом
          }
          if (buttonSelectClicked) { // Проверка на нажатие кнопки Select
            switch(menuPointers.BATTERY_ICON_TYPE) {
              case 0: buttonBackClicked = true; break;
              case 1: currentSettings.BATTERY_ICON_TYPE = 1; saveConfig(currentSettings); buttonBackClicked = true; break;
              case 2: currentSettings.BATTERY_ICON_TYPE = 2; saveConfig(currentSettings); buttonBackClicked = true; break;
              case 3: currentSettings.BATTERY_ICON_TYPE = 3; saveConfig(currentSettings); buttonBackClicked = true; break;
            }
          }

          // Обработка экрана
          if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
            u8g2.clearBuffer();
            drawListMenu(u8g2, menuItemNames.BATTERY_ICON_TYPE, menuIconNames.BATTERY_ICON_TYPE, menuItemCount.BATTERY_ICON_TYPE, menuPointers.BATTERY_ICON_TYPE);
            u8g2.sendBuffer();
          }
        } buttonBackClicked = false; break;
        case 3: while (!buttonBackClicked) {
          handleButtons();

          if (buttonUpClicked) { // Проверка на нажатие кнопки Up
            menuPointers.STANDBY_TIMEOUT = constrain(menuPointers.STANDBY_TIMEOUT - 1, 0, menuItemCount.STANDBY_TIMEOUT - 1); // Отнимаем у выбранного пункта 1 с лимитом
          }
          if (buttonDownClicked) { // Проверка на нажатие кнопки Down
            menuPointers.STANDBY_TIMEOUT = constrain(menuPointers.STANDBY_TIMEOUT + 1, 0, menuItemCount.STANDBY_TIMEOUT - 1); // Добавляем к выбранному пункту 1 с лимитом
          }
          if (buttonSelectClicked) { // Проверка на нажатие кнопки Select
            switch(menuPointers.STANDBY_TIMEOUT) {
              case 0: buttonBackClicked = true; break;
              case 1: currentSettings.STANDBY_TIMEOUT = 0; saveConfig(currentSettings); buttonBackClicked = true; break;
              case 2: currentSettings.STANDBY_TIMEOUT = 30000; saveConfig(currentSettings); buttonBackClicked = true; break;
              case 3: currentSettings.STANDBY_TIMEOUT = 1 * 60000; saveConfig(currentSettings); buttonBackClicked = true; break;
              case 4: currentSettings.STANDBY_TIMEOUT = 5 * 60000; saveConfig(currentSettings); buttonBackClicked = true; break;
            }
          }

          // Обработка экрана
          if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
            u8g2.clearBuffer();
            drawListMenu(u8g2, menuItemNames.STANDBY_TIMEOUT, menuIconNames.STANDBY_TIMEOUT, menuItemCount.STANDBY_TIMEOUT, menuPointers.STANDBY_TIMEOUT);
            u8g2.sendBuffer();
          }
        } buttonBackClicked = false; break;
        case 4: while (!buttonBackClicked) {
          handleButtons();

          if (buttonUpClicked) { // Проверка на нажатие кнопки Up
            menuPointers.SPLASH_TIMEOUT = constrain(menuPointers.SPLASH_TIMEOUT - 1, 0, menuItemCount.SPLASH_TIMEOUT - 1); // Отнимаем у выбранного пункта 1 с лимитом
          }
          if (buttonDownClicked) { // Проверка на нажатие кнопки Down
            menuPointers.SPLASH_TIMEOUT = constrain(menuPointers.SPLASH_TIMEOUT + 1, 0, menuItemCount.SPLASH_TIMEOUT - 1); // Добавляем к выбранному пункту 1 с лимитом
          }
          if (buttonSelectClicked) { // Проверка на нажатие кнопки Select
            switch(menuPointers.SPLASH_TIMEOUT) {
              case 0: buttonBackClicked = true; break;
              case 1: currentSettings.SPLASH_TIMEOUT = 0; saveConfig(currentSettings); buttonBackClicked = true; break;
              case 2: currentSettings.SPLASH_TIMEOUT = 250; saveConfig(currentSettings); buttonBackClicked = true; break;
              case 3: currentSettings.SPLASH_TIMEOUT = 500; saveConfig(currentSettings); buttonBackClicked = true; break;
              case 4: currentSettings.SPLASH_TIMEOUT = 1 * 1000; saveConfig(currentSettings); buttonBackClicked = true; break;
            }
          }

          // Обработка экрана
          if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
            u8g2.clearBuffer();
            drawListMenu(u8g2, menuItemNames.SPLASH_TIMEOUT, menuIconNames.SPLASH_TIMEOUT, menuItemCount.SPLASH_TIMEOUT, menuPointers.SPLASH_TIMEOUT);
            u8g2.sendBuffer();
          }
        } buttonBackClicked = false; break;
      }
    }

    if (buttonUpClicked) {         // Проверка на нажатие кнопки Up
      menuPointers.SETTINGS = constrain(menuPointers.SETTINGS - 1, 0, menuItemCount.SETTINGS - 1); // Отнимаем у выбранного пункта 1 с лимитом
    }
    if (buttonDownClicked) {       // Проверка на нажатие кнопки Down
      menuPointers.SETTINGS = constrain(menuPointers.SETTINGS + 1, 0, menuItemCount.SETTINGS - 1); // Добавляем к выбранному пункту 1 с лимитом
    }

    // Обработка экрана
    if (millis() - lastFrameTime >= 1000 / OLED_FPS) {
      u8g2.clearBuffer();
      drawListMenu(u8g2, menuItemNames.SETTINGS, menuIconNames.SETTINGS, menuItemCount.SETTINGS, menuPointers.SETTINGS);
      u8g2.sendBuffer();
    }
  } buttonBackClicked = false;
}
void mainMenu() {            
  handleBattery();               // Обновляет информацию баттареи
  handleButtons();               // Обновляет информацию кнопок

  if (buttonUpClicked) {         // Проверка на нажатие кнопки Up
    menuPointers.MAIN = constrain(menuPointers.MAIN - 1, 0, menuItemCount.MAIN - 1); // Отнимаем у выбранного пункта 1 с лимитом
  }
  if (buttonDownClicked) {       // Проверка на нажатие кнопки Down
    menuPointers.MAIN = constrain(menuPointers.MAIN + 1, 0, menuItemCount.MAIN - 1); // Добавляем к выбранному пункту 1 с лимитом
  }
  if (buttonSelectClicked) {     // Проверка на нажатие кнопки Select
    switch(menuPointers.MAIN) {
      case 0: wifiMenu(); break;
      case 1: break;
      case 2: break;
      case 3: aboutScreen(); break;
      case 4: settingsScreen(); break;
    }
  }

  if (millis() - lastFrameTime >= 1000 / OLED_FPS) { // Проверка на FPS
    u8g2.clearBuffer();          // Очистка буффера
    if (millis() - lastActionTime < currentSettings.STANDBY_TIMEOUT || !currentSettings.STANDBY_TIMEOUT) {
      drawBattery(u8g2);         // Отрисовка аккумулятора
      switch(currentSettings.MAIN_MENU_TYPE) { // Отрисовка меню с соответствующим типом
        case 1: drawClassicMainMenu(u8g2, menuItemNames.MAIN, menuIconNames.MAIN, menuItemCount.MAIN, menuPointers.MAIN); break; // Classic Main Menu
        case 2: drawStationMainMenu(u8g2, menuItemNames.MAIN, menuIconNames.MAIN, menuItemCount.MAIN, menuPointers.MAIN); break; // Station Main Menu
      }
    } else {
      drawStandbyAnimation(u8g2);
    }
    u8g2.sendBuffer();
    lastFrameTime = millis();
  }
}
