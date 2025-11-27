#ifndef CONFIG_H 
#define CONFIG_H 

// Библиотеки
#include <Arduino.h>                       // Импорт библиотеки для работы базовых функций
#include <U8g2lib.h>                       // Импорт библиотеки для OLED экрана/дисплея

// Файлы
#include "bitmap.h"

// Экран 
const int OLED_FPS = 30;                   // Количество кадров в секунду
const int OLED_SDA = 6;                    // Пин SDA
const int OLED_SCL = 7;                    // Пин SCL/SCK
const int OLED_RESET = U8X8_PIN_NONE;      // Пин сброса
const uint8_t OLED_CONTRAST = 255;         // Контрастность/яркость
const u8g2_cb_t *OLED_ROTATION = U8G2_R0;  // Ориентация

// Интерфейс
// const unsigned long BOOT_LOGO_DELAY = 1000; // Время показа логотипа при запуске (0 если отключено) | Можно изменить в  настройках
const uint8_t *UI_FONT = u8g2_font_profont11_tr; // Шрифт
const char* MAIN_MENU_ITEMS[] = {"Wi-Fi", "Bluetooth", "BadUSB/BLE", "About", "Settings"}; // Название пунктов главного меню
const char* WIFI_MENU_ITEMS[] = {"Back", "Packet Monitor", "Beacon Spam", "Deauther"}; // Название пунктов Wi-Fi меню
const char* BEACON_SPAM_MENU_ITEMS[] = {"Back", "Rickroll Spam", "Random Spam", "Funny Spam"}; // Название пунктов Beacon Spam меню
const char* SETTINGS_MENU_ITEMS[] = {"Back", "Interface"}; // Название пунктов Settings меню
const char* INTERFACE_SETTINGS_MENU_ITEMS[] = {"Back", "Main Menu Type", "Standby Timeout", "Boot Logo Delay"}; // Название пунктов Interface Settings меню
const char* INTERFACE_MAIN_MENU_TYPE_SETTINGS_MENU_ITEMS[] = {"Back", "Havoc (default)", "Station"}; // Название пунктов Interface Main Menu Type Settings меню
const char* INTERFACE_STANDBY_TIMEOUT_SETTINGS_MENU_ITEMS[] = {"Back", "Disabled", "30 Sec", "1 Min", "5 Min (default)", "15 Min", "30 Min"}; // Название пунктов Interface Standby Timeout Settings меню
const char* INTERFACE_BOOT_LOGO_DELAY_SETTINGS_MENU_ITEMS[] = {"Back", "Disabled", "250 Ms", "500 Ms", "1 Sec (default)", "2 Sec", "5 Sec"}; // Название пунктов Interface Boot Logo Delay Settings меню
const unsigned char* MAIN_MENU_ICONS[] = {bitmap_wifi_icon, bitmap_bluetooth_icon, bitmap_usb_icon, bitmap_about_icon, bitmap_settings_icon};
const unsigned char* WIFI_MENU_ICONS[] = {bitmap_small_back_icon, bitmap_small_packetmonitor_icon, bitmap_small_wifi_icon, bitmap_small_skull_icon};
const unsigned char* BEACON_SPAM_MENU_ICONS[] = {bitmap_small_back_icon, bitmap_small_wifi_icon, bitmap_small_wifi_icon, bitmap_small_wifi_icon};
const unsigned char* SETTINGS_MENU_ICONS[] = {bitmap_small_back_icon, bitmap_small_interface_icon};
const unsigned char* INTERFACE_SETTINGS_MENU_ICONS[] = {bitmap_small_back_icon, bitmap_small_interface_icon, bitmap_small_interface_icon, bitmap_small_interface_icon, bitmap_small_interface_icon};
const unsigned char* INTERFACE_MAIN_MENU_TYPE_SETTINGS_MENU_ICONS[] = {bitmap_small_back_icon, bitmap_small_interface_icon, bitmap_small_interface_icon, bitmap_small_interface_icon};
const unsigned char* INTERFACE_STANDBY_TIMEOUT_SETTINGS_MENU_ICONS[] = {bitmap_small_back_icon, bitmap_small_interface_icon, bitmap_small_interface_icon, bitmap_small_interface_icon, bitmap_small_interface_icon, bitmap_small_interface_icon, bitmap_small_interface_icon};
const unsigned char* INTERFACE_BOOT_LOGO_DELAY_SETTINGS_MENU_ICONS[] = {bitmap_small_back_icon, bitmap_small_interface_icon, bitmap_small_interface_icon, bitmap_small_interface_icon, bitmap_small_interface_icon, bitmap_small_interface_icon, bitmap_small_interface_icon};

// Кнопки
const int BUTTON_UP = 4;                   // Кнопка Up/Вверх
const int BUTTON_DOWN = 3;                 // Кнопка Down/Вниз
const int BUTTON_SELECT = 2;               // Кнопка Select/Выбор
const int BUTTON_BACK = 1;                 // Кнопка Back/Назад
const unsigned long DEBOUNCE_DELAY = 250;  // Задержка антидребезга

// Анимация ожидания
const unsigned long STANDBY_TIMEOUT = 30000; // Время ожидания анимации | Можно изменить в  настройках

// Батарея
const int ADC_RESOLUTION = 4095;           // Разрешение АЦП
const int VOLTAGE_PIN = 0;                 // Пин измерения напряжения (К нему нежно подключить плюс и минус батареи через 10кОм резисторы)
const float VOLTAGE_REF = 3.3;             // Опорное напряжение АЦП
const float VOLTAGE_MAX = 4.5;             // Максимальное напряжение батареи
const float VOLTAGE_MIN = 3.00;            // Минимальное напряжение батареи
const float VOLTAGE_SMOOTH = 0.1;          // Коофицент сглаживания
const float VOLTAGE_CHARGE = 4.75;         // Минимальное напряжение для определения зарядки
const float VOLTAGE_MULTIPLIER = 2.0;      // Коофицент делителя

// Другое
const unsigned short SETTINGS_VERSION = 1;   // Билд прошивки

#endif
