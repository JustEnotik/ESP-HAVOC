#pragma once

// Библиотеки
#include <Arduino.h>
#include <U8g2lib.h>

// Файлы
#include "config.h"
#include "bitmap.h"

int batteryPercent = 0;
float batteryVoltage = 0.0;

void handleBattery() {
  int rawAdc = analogRead(VOLTAGE_PIN);
  float vOut = (float)rawAdc / ADC_RESOLUTION * VOLTAGE_REF;
  float currentVoltage = vOut * VOLTAGE_MULTIPLIER;
  
  batteryVoltage = batteryVoltage + VOLTAGE_SMOOTH * (currentVoltage - batteryVoltage);

  float voltageRange = VOLTAGE_MAX - VOLTAGE_MIN;
  float voltageRelative = batteryVoltage - VOLTAGE_MIN;
  
  float percentFloat = (voltageRelative / voltageRange) * 100.0;
  
  batteryPercent = constrain(round(percentFloat), 0, 100);
}

void getPercentStr(char* buffer) {
  sprintf(buffer, "%d %%", batteryPercent);
}

void getVoltageStr(char* buffer) {
  dtostrf(batteryVoltage, 4, 2, buffer);
}