#ifndef SETTINGS_H 
#define SETTINGS_H 

// Библиотеки
#include <Arduino.h>
#include <U8g2lib.h>

// Файлы
#include "config.h"

struct {
  unsigned short MAIN_MENU_TYPE = 1; // 1 - Havoc/Classic, 2 - Station
  unsigned long STANDBY_TIMEOUT = 300000;
  unsigned long BOOT_LOGO_DELAY = 1000;
} settings;

#endif