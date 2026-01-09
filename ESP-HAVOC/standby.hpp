// Что бы не было ошибок
#pragma once                    // В душе не чаю

// Библиотеки
#include <Arduino.h>            // Базовые функции/фишки Arduino
#include <U8g2lib.h>            // Вывод на дисплей

// Файлы
#include "config.h"             // Настройки
#include "bitmap.h"             // Битмапы

// Переменные
int dvd_x = 0;                  // Координата X на данный момент
int dvd_y = 0;                  // Координата Y на данный момент
int dvd_velocity_x = 1;         // Направление по X на данный момент
int dvd_velocity_y = 1;         // Направление по Y на данный момент

// Функции
void drawStandbyAnimation(U8G2& u8g2) { // Функция для отрисовки анимации
  // u8g2.clearBuffer();        // Очистка буфера
  u8g2.setDrawColor(1);         // Установка режима отрисовки
  u8g2.drawXBM(                 // Отправка битмапа в буфер
    dvd_x,                      // X битмапа (координата X на данный момент)
    dvd_y,                      // Y битмапа (Координата Y на данный момент)
    DVD_LOGO_WIDTH,             // Ширина битмапа
    DVD_LOGO_HEIGHT,            // Высота битмапа
    DVD_BITMAP                  // Битмап
  );

  dvd_x += dvd_velocity_x * DVD_VELOCITY_MULTIPLIER; // Обновление (смещение) координаты X битмапа на данный момент
  dvd_y += dvd_velocity_y * DVD_VELOCITY_MULTIPLIER; // Обновление (смещение) координаты Y битмапа на данный момент

  if (dvd_x + DVD_LOGO_WIDTH >= u8g2.getDisplayWidth()) { // Проверка на столкнение с правым краем
    dvd_velocity_x = -1;        // Изменение направления координаты X
    dvd_x = u8g2.getDisplayWidth() - DVD_LOGO_WIDTH; // Изменение координаты X
  }
  else if (dvd_x <= 0) {        // Проверка на столкнение с левым краем
    dvd_velocity_x = 1;         // Изменение направления координаты X
    dvd_x = 0;                  // Изменение координаты X
  }
  if (dvd_y + DVD_LOGO_HEIGHT >= u8g2.getDisplayHeight()) { // Проверка на столкнение с верхним краем
    dvd_velocity_y = -1;        // Изменение направления координаты Y
    dvd_y = u8g2.getDisplayHeight() - DVD_LOGO_HEIGHT; // Изменение координаты Y
  }
  else if (dvd_y <= 0) {        // Проверка на столкнение с нижним краем
    dvd_velocity_y = 1;         // Изменение направления координаты Y
    dvd_y = 0;                  // Изменение координаты Y
  }

  // u8g2.sendBuffer();         // Отправка в буфер
}
