// Что бы не было ошибок
#pragma once // В душе не чаю

// Библиотеки
#include <Arduino.h> // Базовые функции/фишки Arduino
#include <U8g2lib.h> // Вывод на дисплей

// Файлы
#include "bitmap.h" // Битмапы

// Экран 
const unsigned short OLED_FPS = 30; // Количество кадров в секунду
const unsigned short OLED_SDA = 8; // Пин SDA (I2C data)
const unsigned short OLED_SCL = 9; // Пин SCL/SCK (I2C clock)
const unsigned short OLED_RESET = U8X8_PIN_NONE; // Пин сброса (I2C reset)
const uint8_t OLED_CONTRAST = 255; // Контрастность/яркость

// Интерфейс
// unsigned long SPLASH_DELAY = 1000; // Время показа логотипа при запуске (0 если отключено)
const uint8_t *UI_FONT = u8g2_font_profont11_tr; // Шрифт
const u8g2_cb_t *UI_ROTATION = U8G2_R0; // Ориентация

// Меню
struct MENU_ITEM_NAMES {
  const char* MAIN[5] = {"Wi-Fi", "Bluetooth", "BadUSB", "About", "Settings"};
  const char* WIFI[4] = {"Back", "Deauth Flood", "Beacon Flood", "Packet Monitor"};
  const char* BEACON_FLOOD[4] = {"Back", "Funny Flood", "Ricky Flood", "Random Flood"};
  // const char* BLUETOOTH[] = {};
  // const char* ABOUT[] = {};
  const char* SETTINGS[5] = {"Back", "Main Menu Type", "Battery Icon Type", "Standby Timeout", "Splash Timeout"};
  const char* BATTERY_ICON_TYPE[4] = {"Back", "Disabled", "4 Bars (default)", "3 Bars"};
  const char* MAIN_MENU_TYPE[4] = {"Back", "Havoc (default)", "Station"};
  const char* STANDBY_TIMEOUT[5] = {"Back", "Disabled", "30 Sec", "1 Min (default)", "5 Min"};
  const char* SPLASH_TIMEOUT[5] = {"Back", "Disabled", "250 Ms", "500 Ms (default)", "1 Sec"};
};
struct MENU_ICON_NAMES {
  const unsigned char* MAIN[5] = {bitmap_wifi, bitmap_bluetooth, bitmap_usb, bitmap_about, bitmap_settings};
  const unsigned char* WIFI[4] = {bitmap_small_back, bitmap_small_skull, bitmap_small_wifi, bitmap_small_packet_monitor};
  const unsigned char* BEACON_FLOOD[4] = {bitmap_small_back, bitmap_small_wifi, bitmap_small_wifi, bitmap_small_question};
  // const unsigned char* BLUETOOTH[] = {};
  // const unsigned char* ABOUT[] = {};
  const unsigned char* SETTINGS[5] = {bitmap_small_back, bitmap_small_interface, bitmap_small_battery, bitmap_small_watch, bitmap_small_watch};
  const unsigned char* BATTERY_ICON_TYPE[4] = {bitmap_small_back, bitmap_small_battery, bitmap_small_battery, bitmap_small_battery};
  const unsigned char* MAIN_MENU_TYPE[3] = {bitmap_small_back, bitmap_small_interface, bitmap_small_interface};
  const unsigned char* STANDBY_TIMEOUT[4] = {bitmap_small_back, bitmap_small_watch, bitmap_small_watch, bitmap_small_watch};
  const unsigned char* SPLASH_TIMEOUT[4] = {bitmap_small_back, bitmap_small_watch, bitmap_small_watch, bitmap_small_watch};
};
struct MENU_ITEM_COUNT {
  const unsigned short MAIN = 5;
  const unsigned short WIFI = 4;
  const unsigned short BEACON_FLOOD = 4;
  // const unsigned short BLUETOOTH = sizeof(MENU_ITEM_NAMES.BLUETOOTH) / sizeof(MENU_ITEM_NAMES.BLUETOOTH[0]);
  // const unsigned short ABOUT = sizeof(MENU_ITEM_NAMES.ABOUT) / sizeof(MENU_ITEM_NAMES.ABOUT[0]);
  const unsigned short SETTINGS = 5;
  const unsigned short BATTERY_ICON_TYPE = 4;
  const unsigned short MAIN_MENU_TYPE = 3;
  const unsigned short STANDBY_TIMEOUT = 4;
  const unsigned short SPLASH_TIMEOUT = 4;
};
struct MENU_POINTERS {
  unsigned short MAIN = 0;
  unsigned short WIFI = 0;
  unsigned short BEACON_FLOOD = 0;
  // unsigned short BLUETOOTH = 0;
  // unsigned short ABOUT = 0;
  unsigned short SETTINGS = 0;
  unsigned short BATTERY_ICON_TYPE = 0;
  unsigned short MAIN_MENU_TYPE = 0;
  unsigned short STANDBY_TIMEOUT = 0;
  unsigned short SPLASH_TIMEOUT = 0;
};

MENU_ITEM_NAMES menuItemNames;
MENU_ICON_NAMES menuIconNames;
MENU_ITEM_COUNT menuItemCount;
MENU_POINTERS menuPointers;

// Кнопки
const unsigned short BUTTON_UP = 2; // Кнопка Up/Вверх
const unsigned short BUTTON_DOWN = 3; // Кнопка Down/Вниз
const unsigned short BUTTON_SELECT = 4; // Кнопка Select/Выбор
const unsigned short BUTTON_BACK = 5; // Кнопка Back/Назад
const unsigned long DEBOUNCE_DELAY = 250; // Задержка антидребезга

// Анимация ожидания
// unsigned long STANDBY_TIMEOUT = 30000; // Время ожидания анимации

// Батарея
const unsigned short ADC_RESOLUTION = 4095; // Разрешение АЦП
const unsigned short VOLTAGE_PIN = 1; // Пин измерения напряжения (К нему нежно подключить 5V и GND пин через 10кОм резисторы)
const float VOLTAGE_REF = 3.3; // Опорное напряжение АЦП
const float VOLTAGE_MAX = 4.0; // Максимальное напряжение батареи
const float VOLTAGE_MIN = 3.5; // Минимальное напряжение батареи (При питании на пин 5V)
const float VOLTAGE_SMOOTH = 0.1; // Коофицент сглаживания
const float VOLTAGE_MULTIPLIER = 2.0; // Коофицент делителя

// Прошивка
// const unsigned short SETTINGS_VERSION = 1; // Версия настроек
const char* FIRMWARE_VERSION = "Stable Release 1.1.0"; // Версия прошивки
const char* BUILD_DATE = "01.01.70"; // Дата билда прошивки

// Файлы
constexpr char CONFIG_FILE_PATH[] = "/.esphavoc/config.bin";

// Анимация ожидания
const int DVD_LOGO_WIDTH = 27; // Ширина битмапа
const int DVD_LOGO_HEIGHT = 16; // Высота битмапа
const int DVD_VELOCITY_MULTIPLIER = 1; // Множитель направлений по координатам (множитель скорости)
const unsigned char* DVD_BITMAP = bitmap_standby; // Битмап

// Структура настроеек для сохранения в LittleFS (EEPROM)
struct settings {
  unsigned short BATTERY_ICON_TYPE = 2; // Тип главного меню (1 - Disabled, 2 - 4 Bars, 3 - 3 Bars)
  unsigned short MAIN_MENU_TYPE = 1; // Тип главного меню (1 - Havoc/Classic, 2 - Station)
  unsigned long STANDBY_TIMEOUT = 30000; // Тайм-аут режима ожидания
  unsigned long SPLASH_TIMEOUT = 500; // Заддержка логотипа при запуске
};
settings defaultSettings;
settings currentSettings;
