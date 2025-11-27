#pragma once

// Библиотеки
#include <Arduino.h>
#include <U8g2lib.h>

// Файлы
#include "bitmap.h"
#include "config.h"

void bufferBattery(
  U8G2& u8g2, 
  float batteryVoltage, 
  int batteryPercent) {
  // Если заряжается 
  if (batteryVoltage > VOLTAGE_CHARGE) u8g2.drawXBM(108, 4, 16, 7, bitmap_charging_battery_icon);
  // Если более 87% (8 баров)
  else if (batteryPercent > 87) u8g2.drawXBM(108, 4, 16, 7, bitmap_8_bar_battery_icon);
  // Если более 75% (7 баров)
  else if (batteryPercent > 75) u8g2.drawXBM(108, 4, 16, 7, bitmap_7_bar_battery_icon);
  // Если более 62% (6 баров)
  else if (batteryPercent > 62) u8g2.drawXBM(108, 4, 16, 7, bitmap_6_bar_battery_icon);
  // Если более 50% (5 баров)
  else if (batteryPercent > 50) u8g2.drawXBM(108, 4, 16, 7, bitmap_5_bar_battery_icon);
  // Если более 37% (4 бара)
  else if (batteryPercent > 37) u8g2.drawXBM(108, 4, 16, 7, bitmap_4_bar_battery_icon);
  // Если более 25% (3 бара)
  else if (batteryPercent > 25) u8g2.drawXBM(108, 4, 16, 7, bitmap_3_bar_battery_icon);
  // Если более 12% (2 бара)
  else if (batteryPercent > 12) u8g2.drawXBM(108, 4, 16, 7, bitmap_2_bar_battery_icon);
  // Если 12% и/или меньше (1 бар)
  else u8g2.drawXBM(108, 4, 16, 7, bitmap_1_bar_battery_icon);
}

void bufferMainMenu(
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
  u8g2.drawStr((128 - (strlen(items[selectedItem]) * 6)) / 2, 48, items[selectedItem]);
  // Рамка выбранного пункта
  u8g2.setDrawColor(2);
  u8g2.drawBox(selectedItem * 20 + 15, 20, 18, 18);
}
void bufferStationMainMenu(
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
  u8g2.drawXBM(28, 41, 8, 8, bitmap_small_arrows_icon);

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
void bufferPacketMonitorMenu(
  U8G2& u8g2, 
  int channel, 
  int packetsTotal,
  int packetsDeauth,
  int packetsBeacon,
  uint32_t* packetsHistory) {

  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(UI_FONT);

  char buf[3];

  for (int i = 0; i < 128; i++) {
    int y = constrain(map(packetsHistory[i], 0, 100, 64, 16), 16, 64);
    u8g2.drawLine(i, 64, i, y);  
  }

  // Иконка Skull
  u8g2.drawXBM(96, 4, 8, 8, bitmap_small_skull_icon);

  // Кол-во пакетов (Deauth)
  u8g2.setFont(UI_FONT);
  u8g2.drawStr(108, 12, (sprintf(buf, "%03d", constrain(packetsDeauth, 0, 999)), buf));

  // Иконка Packet Monitor
  u8g2.drawXBM(30, 4, 8, 8, bitmap_small_packetmonitor_icon);

  // Кол-во пакетов (Все)
  u8g2.drawStr(42, 12, (sprintf(buf, "%03d", constrain(packetsTotal, 0, 999)), buf));

  // Иконка Wi-Fi
  u8g2.drawXBM(63, 4, 8, 8, bitmap_small_wifi_icon);

  // Канал 
  u8g2.drawStr(14, 12, (sprintf(buf, "%02d", constrain(channel, 0, 999)), buf));

  // Иконка Arrows
  u8g2.drawXBM(3, 4, 8, 8, bitmap_small_arrows_icon);

  // Кол-во пакетов (Beacon)
  u8g2.drawStr(75, 12, (sprintf(buf, "%03d", constrain(packetsBeacon, 0, 999)), buf));
}

void bufferBeaconSpamScreen(
  U8G2& u8g2, 
  const char* item) {
  // Текст
  u8g2.drawStr((128 - (strlen((String(item)).c_str()) * 6)) / 2, 28, (String(item)).c_str());

  // Назад
  u8g2.drawStr(58, 40, "Back");

  // Иконка назад
  u8g2.drawXBM(46, 32, 8, 8, bitmap_small_back_icon);
}

void bufferListMenu (
  U8G2& u8g2, 
  int selectedItem,
  const char* const* items,
  const uint8_t* const* itemsIcons,
  uint8_t itemCount
) {
  // Настройки
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setDrawColor(1);
  u8g2.setFont(UI_FONT);

  int currentPage = selectedItem / 4;

  for (int i = 0; i + currentPage * 4 < itemCount; i++) {
    // Иконка
    u8g2.drawXBM(4, 4 + i * 16, 8, 8, itemsIcons[i + currentPage * 4]);
    // Текст
    u8g2.drawStr(16, 12 + i * 16, items[i + currentPage * 4]);
  }

  // Обводка выбранного пункта
  u8g2.setDrawColor(2);
  u8g2.drawBox(0, (selectedItem % 4) * 16, 128, 16);
}
