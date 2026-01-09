// Взял код с репозитория Theckeegs/KBeacon-Spammer

// Что бы не было ошибок
#pragma once                    // В душе не чаю

// Библиотеки
#include <Arduino.h>            // Базовые функции/фишки Arduino

// Библиотеки ESP32
#include <esp_wifi.h>           // Низкоуровневые Wi-Fi функции/фишки ESP32
#include <WiFi.h>               // Базовые Wi-Fi функции/фишки ESP32
#include <vector>               // Динамический список элементов

// Файлы
#include "screen.hpp"           // Экран и интерфейс

// Массивы, константы и переменные
uint8_t beaconPacket[128];      // Пакет для отправки
uint8_t macAddress[6];          // MAC адрес используемый для отправки пакетов

// unsigned short channels[] = {1, 6, 10, 11, 12}; // Каналы используемые для отправки пакетов
static unsigned short beaconFloodChannel = 1;      // Канал используемый для отправки пакетов
static unsigned short beaconFrameCount = 16;       // Количество пакетов для отправки 1-ого SSID
static unsigned short currentBeaconIndexSSID = 0;
  static const char** currentBeaconSSID = nullptr;

static const char* randomSSID[256]; // Массив указателей
static char randomSSIDBuffer[256][32]; // Буфер
const unsigned short randomSSIDCount = 256; // Количество строк в списке randomSSID

void generateRandomSSIDList() { // Генератор списка рандомных SSID
  const char *charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; // Символы для генерации
  unsigned short len = 16;          // Длинна генерируемых SSID
  
  for (unsigned short ssidIdx = 0; ssidIdx < randomSSIDCount; ssidIdx++) { // Цикл по всем SSID
    for (unsigned short i = 0; i < len; i++) { // Цикл по символам в одном SSID
      randomSSIDBuffer[ssidIdx][i] = charset[esp_random() % strlen(charset)]; // Генерация символа
    }
    randomSSID[ssidIdx] = randomSSIDBuffer[ssidIdx]; // Завершающий нулевой символ строки
  }
}

const char* funnySSID[] PROGMEM = { // Список Funny/Beacon SSID
  "Mom Use This One",
  "Abraham Linksys",
  "Benjamin FrankLAN",
  "Martin Router King",
  "John Wilkes Bluetooth",
  "Pretty Fly for a Wi-Fi",
  "Bill Wi the Science Fi",
  "I Believe Wi Can Fi",
  "Tell My Wi-Fi Love Her",
  "No More Mister Wi-Fi",
  "LAN Solo",
  "The LAN Before Time",
  "Silence of the LANs",
  "House LANister",
  "Winternet Is Coming",
  "Ping's Landing",
  "The Ping in the North",
  "This LAN Is My LAN",
  "Get Off My LAN",
  "The Promised LAN",
  "The LAN Down Under",
  "FBI Surveillance Van 4",
  "Area 51 Test Site",
  "Drive-By Wi-Fi",
  "Planet Express",
  "Wu Tang LAN",
  "Darude LANstorm",
  "Never Gonna Give You Up",
  "Hide Yo Kids, Hide Yo Wi-Fi",
  "Loading…",
  "Searching…",
  "VIRUS.EXE",
  "Virus-Infected Wi-Fi",
  "Starbucks Wi-Fi",
  "Text 64ALL for Password",
  "Yell HAVOC for Password",
  "The Password Is 1234",
  "Free Public Wi-Fi",
  "No Free Wi-Fi Here",
  "Get Your Own Damn Wi-Fi",
  "It Hurts When IP",
  "Dora the Internet Explorer",
  "404 Wi-Fi Unavailable",
  "Porque-Fi",
  "Titanic Syncing",
  "Test Wi-Fi Please Ignore",
  "Drop It Like It's Hotspot",
  "Life in the Fast LAN",
  "The Creep Next Door",
  "Ye Olde Internet",
  // Новые SSID от Gemini ниже
  "The LAN That Time Forgot", 
  "WiFi-Fighter", 
  "I See You", 
  "Connect 4 Less", 
  "Too Legit To Quit", 
  "Wi-Fi Wonka", 
  "Drop It Like It's Hot Spot", 
  "Where My Pitches At?", 
  "Don't Stop Me Now", 
  "The LAN of the Free",
  "You Shall Not Pass!", 
  "Network Not Found", 
  "Please Connect to 5", 
  "Clickbait Wi-Fi", 
  "Wi-Fi Spy", 
  "Get Connected or Die Tryin'", 
  "Error 404: Internet Not Found", 
  "Pennywise's Sewer Connection", 
  "Home Sweet Home Network", 
  "Just Say No To Slow", 
  "Searching for Signal", 
  "The Borg Collective", 
  "Router of the Rings", 
  "Not The Droids You're Looking For"
};
const unsigned short funnySSIDCount = sizeof(funnySSID) / sizeof(funnySSID[0]); // Количество строк в списке funnySSID
const char* rickySSID[] PROGMEM = {     // Список Ricky/Rickroll SSID
  "01 Never gonna give you up",
  "02 Never gonna let you down",
  "03 Never gonna run around",
  "04 and desert you",
  "05 Never gonna make you cry",
  "06 Never gonna say goodbye",
  "07 Never gonna tell a lie",
  "08 and hurt you",
  // Новые SSID от Gemini ниже
  "09 We've known each other", 
  "10 For so long", 
  "11 Your heart's been aching", 
  "12 Too shy to say it", 
  "13 Inside we both know", 
  "14 What's been going on", 
  "15 We know the game", 
  "16 Gonna play it", 
  "17 If you ask me", 
  "18 How I'm feeling", 
  "19 Too blind to see"
};
const unsigned short rickySSIDCount = sizeof(rickySSID) / sizeof(rickySSID[0]); // Количество строк в списке rickySSID

void generateRandomMac(uint8_t* mac) { // Генератор рандомного MAC-ардеса
  for(unsigned short i = 0; i < 6; i++) {  // Цикл генерации
    mac[i] = random(256);       // Задание части MAC-адреса рандомным числом
  }
  mac[0] |= 0x02;               // Я хз, мб так надо
  mac[0] &= 0xFE;               // Я хз, мб так надо
}
void generateBeaconPacket(const String& ssid) { // Генератор пакета для отправки
  uint8_t packet[128] = {
    0x80, 0x00,
    0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x64, 0x00,
    0x11, 0x00,
    0x00
  };

  memset(beaconPacket, 0, sizeof(beaconPacket));
  memcpy(beaconPacket, packet, sizeof(packet));
  
  generateRandomMac(macAddress);
  memcpy(&beaconPacket[10], macAddress, 6);
  memcpy(&beaconPacket[16], macAddress, 6);
  
  beaconPacket[36] = 0x00;
  beaconPacket[37] = ssid.length();
  memcpy(&beaconPacket[38], ssid.c_str(), ssid.length());
}
void drawBeaconFloodScreen(            // Отрисовка меню спама
  U8G2& u8g2, 
  const char* floodName,
  const char* floodLog) {
  char buf[3]; // Буфер для текста
  char truncatedLog[20]; // Буфер для текста логов

  if (strlen(floodLog) > 16) {
    snprintf(truncatedLog, sizeof(truncatedLog), "%.16s...", floodLog);
  } else {
    strncpy(truncatedLog, floodLog, sizeof(truncatedLog) - 1);
    truncatedLog[sizeof(truncatedLog) - 1] = '\0';
  }

  // Текст
  u8g2.drawStr((128 - (strlen((String(floodName)).c_str()) * 6)) / 2, 27, (String(floodName)).c_str());
  u8g2.drawStr((128 - (strlen(truncatedLog) * 6)) / 2, 36, truncatedLog);
  u8g2.drawStr(70, 46, "Back");
  u8g2.drawStr(44, 46, (sprintf(buf, "%02d", constrain(beaconFloodChannel, 0, 14)), buf));

  // Назад
  u8g2.drawXBM(60, 38, 8, 8, bitmap_small_back);

  // Стрелки  
  u8g2.drawXBM(34, 38, 8, 8, bitmap_small_arrows);
}
void handleBeaconFlood(          // Поддерживает работу спама
  const char** SSID,            // Список SSID
  const unsigned short countSSID // Количество строк в списке SSID
) {
  if (currentBeaconSSID != SSID || currentBeaconIndexSSID >= countSSID - 1) {    // Первый запуск (или запуск с последним в спискек) с данным списком SSID
    currentBeaconSSID = SSID;
    currentBeaconIndexSSID = 0;
  } else {
    currentBeaconIndexSSID = constrain(currentBeaconIndexSSID + 1, 0, countSSID - 1);
  }

  // for (unsigned short ch = 0; ch < 3; ch++) {
  esp_wifi_set_channel(beaconFloodChannel, WIFI_SECOND_CHAN_NONE); // Установка канала

  // for (unsigned short i = 0; i < countSSID; i++) { // Цикл отправки всех SSID (чут чут блокирующий)
  for (int i = 0; i < beaconFrameCount; i++) {
    currentBeaconIndexSSID++;
    if (currentBeaconIndexSSID >= countSSID) currentBeaconIndexSSID = 0;

    String ssidString = String(SSID[currentBeaconIndexSSID]);
    generateBeaconPacket(ssidString); 
    unsigned short packetSize = 38 + ssidString.length();
    
    esp_wifi_80211_tx(WIFI_IF_AP, beaconPacket, packetSize, false);
  }
  // yield();                  // Даем микроконтроллеру отдохнуть
    // setupScreenTimers();
  // }
  // }
}
void setupBeaconFlood() {        
  randomSeed(esp_random());
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  esp_wifi_set_mode(WIFI_MODE_AP);
}

void teardownBeaconFlood() {
  esp_wifi_set_promiscuous(false);
  ESP_ERROR_CHECK(esp_wifi_stop());
  ESP_ERROR_CHECK(esp_wifi_deinit());
  
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  
  memset(beaconPacket, 0, sizeof(beaconPacket));
  memset(macAddress, 0, sizeof(macAddress));
  currentBeaconSSID = nullptr;
}
