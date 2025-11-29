#pragma once

// Библиотеки
#include <Arduino.h>
#include <U8g2lib.h>

// Файлы
#include "bitmap.h"

// Размеры логотипа
int DVD_LOGO_WIDTH = 27;
int DVD_LOGO_HEIGHT = 16;

// Начальная координата
int dvd_x = 0;
int dvd_y = 0;
// Начальная траектория
int dvd_dx = 1; 
int dvd_dy = 1; 

// Скорость
const int DVD_SPEED = 2;

// Обьявление функции отрисовки анимации
void drawStandbyAnimation(U8G2& u8g2) {
  u8g2.clearBuffer();
  u8g2.setDrawColor(1);
  u8g2.drawXBM(dvd_x, dvd_y, DVD_LOGO_WIDTH, DVD_LOGO_HEIGHT, bitmap_dvd_logo);

  dvd_x += dvd_dx * DVD_SPEED;
  dvd_y += dvd_dy * DVD_SPEED;

  // Влево
  if (dvd_x + DVD_LOGO_WIDTH >= u8g2.getDisplayWidth()) {
    dvd_dx = -1;
    dvd_x = u8g2.getDisplayWidth() - DVD_LOGO_WIDTH; 
  }
  // Вправо
  else if (dvd_x <= 0) {
    dvd_dx = 1;
    dvd_x = 0;
  }
  // Вверх
  if (dvd_y + DVD_LOGO_HEIGHT >= u8g2.getDisplayHeight()) {
    dvd_dy = -1;
    dvd_y = u8g2.getDisplayHeight() - DVD_LOGO_HEIGHT;
  }
  // Вниз
  else if (dvd_y <= 0) {
    dvd_dy = 1;
    dvd_y = 0;
  }

  u8g2.sendBuffer();
}