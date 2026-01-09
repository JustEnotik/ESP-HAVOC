// Защита от повторного включения
#pragma once
// Библиотеки
#include <Arduino.h>
#include <U8g2lib.h>
// Файлы
#include "config.h"
#include "bitmap.h"

// Переменные
int batteryPercent = 0;
float batteryVoltage = 0.0;
bool batteryCharging = false;

// Инициализация батареи
void setupBattery() {
  pinMode(VOLTAGE_PIN, INPUT);
}

// Вычисление заряда батареи
void handleBattery() {
  // Чтение и преобразование напряжения в одну строку
  float currentVoltage = (analogRead(VOLTAGE_PIN) / (float)ADC_RESOLUTION) * VOLTAGE_REF * VOLTAGE_MULTIPLIER;
  
  // Сглаживание напряжения
  batteryVoltage += VOLTAGE_SMOOTH * (currentVoltage - batteryVoltage);

  // Вычисление процента с ограничением диапазона
  batteryPercent = constrain(round(((batteryVoltage - VOLTAGE_MIN) / (VOLTAGE_MAX - VOLTAGE_MIN)) * 100.0), 0, 100);
}

// Отрисовка батареи
void drawBattery(U8G2& u8g2) {
  switch (currentSettings.BATTERY_ICON_TYPE) { // Выбор правильного типа
    case 1: break; // Отключена
    case 2: switch (batteryPercent) {   // Выбор правильного битмапа с 4-мя пунктами если подключен аккумулятор к пину
      case 1 ... 25: u8g2.drawXBM(108, 4, 16, 7, battery_4_low); break; // Отрисовка разряженой батареи
      case 26 ... 50: u8g2.drawXBM(108, 4, 16, 7, battery_4_middle); break; // Отрисовка на половину заряженой баттареи
      case 51 ... 75: u8g2.drawXBM(108, 4, 16, 7, battery_4_high); break; // Отрисовка почти полностью заряженой баттареи
      case 76 ... 100: u8g2.drawXBM(108, 4, 16, 7, battery_4_full); break; // Отрисовка полностью заряженой баттареи
    } break;
    case 3: switch (batteryPercent) {   // Выбор правильного битмапа с 3-мя пунктами если подключен аккумулятор к пину
      case 1 ... 33: u8g2.drawXBM(108, 4, 16, 7, battery_3_low); break; // Отрисовка разряженой батареи
      case 34 ... 66: u8g2.drawXBM(108, 4, 16, 7, battery_3_middle); break; // Отрисовка на половину заряженой баттареи
      case 67 ... 100: u8g2.drawXBM(108, 4, 16, 7, battery_3_full); break; // Отрисовка полностью заряженой баттареи
    } break;
  }
}
