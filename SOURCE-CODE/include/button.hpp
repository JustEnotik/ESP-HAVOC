// Что бы не было ошибок
#pragma once                    // В душе не чаю

// Библиотеки
#include <Arduino.h>            // Базовые функции/фишки Arduino

// Файлы
#include "config.h"             // Настройки

// Переменные
bool buttonUpClicked = false;   // Нажата ли кнопка Up
bool buttonDownClicked = false; // Нажата ли кнопка Down
bool buttonSelectClicked = false; // Нажата ли кнопка Select
bool buttonBackClicked = false; // Нажата ли кнопка Back
unsigned long lastUpDebounceTime = 0; // Последнее время когда кнопка Up была нажата
unsigned long lastDownDebounceTime = 0; // Последнее время когда кнопка Down была нажата
unsigned long lastSelectDebounceTime = 0; // Последнее время когда кнопка Select была нажата
unsigned long lastBackDebounceTime = 0; // Последнее время когда кнопка Back была нажата
unsigned long lastActionTime = 0; // Последнее время Действия

// Функции
void setupButtonTimers() { // Инициализирует таймеры кнопок
  lastUpDebounceTime = millis(); // Последнее время когда кнопка Up была нажата
  lastDownDebounceTime = millis(); // Последнее время когда кнопка Down была нажата
  lastSelectDebounceTime = millis(); // Последнее время когда кнопка Select была нажата
  lastBackDebounceTime = millis(); // Последнее время когда кнопка Back была нажата
  lastActionTime = millis();  // Делаем актуальной проверку на действие
}
void setupButtonPins() { // Инициализирует пины кнопок
  pinMode(BUTTON_UP, INPUT_PULLUP); // Пин кнопки Up
  pinMode(BUTTON_DOWN, INPUT_PULLUP); // Пин кнопки Down
  pinMode(BUTTON_SELECT, INPUT_PULLUP); // Пин кнопки Select
  pinMode(BUTTON_BACK, INPUT_PULLUP); // Пин кнопки Back
}
void handleButtons() { // Поддерживает работу опроса кнопок
  if (                          // Если
    digitalRead(BUTTON_UP) == LOW && // Кнопка Up нажата
    (millis() - lastUpDebounceTime) > DEBOUNCE_DELAY) { // И проходит проверку на антидребезг
    if (millis() - lastActionTime < currentSettings.STANDBY_TIMEOUT || !currentSettings.STANDBY_TIMEOUT) {
      buttonUpClicked = true;     // Изменяем переменную
    }
    lastUpDebounceTime = millis(); // Делаем актуальной проверку на антидребезг
    lastActionTime = millis();  // Делаем актуальной проверку на действие
  } else {                      // Иначе (то есть не нажата)
    buttonUpClicked = false;    // Изменяем переменную
  }
  if (                          // Если
    digitalRead(BUTTON_DOWN) == LOW && // Кнопка Down нажата
    (millis() - lastDownDebounceTime) > DEBOUNCE_DELAY) { // И проходит проверку на антидребезг
    if (millis() - lastActionTime < currentSettings.STANDBY_TIMEOUT || !currentSettings.STANDBY_TIMEOUT) {
      buttonDownClicked = true;   // Изменяем переменную
    }
    lastDownDebounceTime = millis(); // Делаем актуальной проверку на антидребезг
    lastActionTime = millis();  // Делаем актуальной проверку на действие
  } else {                      // Иначе (то есть не нажата)
    buttonDownClicked = false;  // Изменяем переменную
  }
  if (                          // Если
    digitalRead(BUTTON_SELECT) == LOW && // Кнопка Select нажата
    (millis() - lastSelectDebounceTime) > DEBOUNCE_DELAY) { // И проходит проверку на антидребезг
    if (millis() - lastActionTime < currentSettings.STANDBY_TIMEOUT || !currentSettings.STANDBY_TIMEOUT) {
      buttonSelectClicked = true; // Изменяем переменную
    }
    lastSelectDebounceTime = millis(); // Делаем актуальной проверку на антидребезг
    lastActionTime = millis();  // Делаем актуальной проверку на действие
  } else {                      // Иначе (то есть не нажата)
    buttonSelectClicked = false; // Изменяем переменную
  }
  if (                          // Если
    digitalRead(BUTTON_BACK) == LOW && // Кнопка Back нажата
    (millis() - lastBackDebounceTime) > DEBOUNCE_DELAY) { // И проходит проверку на антидребезг
    if (millis() - lastActionTime < currentSettings.STANDBY_TIMEOUT || !currentSettings.STANDBY_TIMEOUT) {
      buttonBackClicked = true;   // Изменяем переменную
    }
    lastBackDebounceTime = millis(); // Делаем актуальной проверку на антидребезг
    lastActionTime = millis();  // Делаем актуальной проверку на действие
  } else {                      // Иначе (то есть не нажата)
    buttonBackClicked = false;  // Изменяем переменную
  }
}
void clickButtonBack() {
  if (                          // Если
    (millis() - lastUpDebounceTime) > DEBOUNCE_DELAY) { // проходит проверку на антидребезг
    if (millis() - lastActionTime < currentSettings.STANDBY_TIMEOUT || !currentSettings.STANDBY_TIMEOUT) {
      buttonUpClicked = true;     // Изменяем переменную
    }
    lastUpDebounceTime = millis(); // Делаем актуальной проверку на антидребезг
    lastActionTime = millis();  // Делаем актуальной проверку на действие
  } else {                      // Иначе (то есть не нажата)
    buttonUpClicked = false;    // Изменяем переменную
  }
}
