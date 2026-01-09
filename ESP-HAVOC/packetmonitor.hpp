// Взял код с репозитория spacehuhn/PacketMonitor32

// Что бы не было ошибок
#pragma once                    // В душе не чаю

// Библиотеки
#include <Arduino.h>            // Базовые функции/фишки Arduino
#include <U8g2lib.h>            // Вывод на дисплей

// Библиотеки ESP32
#include <esp_wifi.h>           // Низкоуровневые Wi-Fi функции/фишки ESP32
#include <esp_netif.h>          // Сетевые интерфейсы/IP-адреса ESP32
#include <esp_event.h>          // Система обработки событий ESP32
#include <esp_system.h>         // Низкоуровневые системные функции ESP32
#include <esp_event_loop.h>     // Цикл событий ESP32
#include <WiFi.h>               // Базовые Wi-Fi функции/фишки ESP32
// #include <esp_wifi_types.h>
// #include <esp_private/wifi.h>

// Переменные
int packetMonitorChannel = 1;
uint32_t detectedPacketsHistory[128]; // История найденных пакетов
uint32_t detectedPacketsDeauth = 0; // Найденные пакеты дезаунтедификации
uint32_t detectedPacketsBeacon = 0; // Найденные пакеты притсутствия сети
uint32_t detectedPacketsTotal = 0; // Найденные пакеты
uint32_t detectedPacketsTemp = 0; // Найденные временные пакеты
unsigned long detectedPacketsPerSecond = 0; // Найденные пакеты за секунду

// Заголовки кадров Wi-Fi
typedef struct {
  // Frame Control (Управление Кадром)
  uint16_t proto: 2;            // Версия протокола (всегда 0)
  uint16_t type: 2;             // Тип кадра (Управление, Контроль, Данные)
  uint16_t subtype: 4;          // Подтип кадра (Beacon, Data и т.д.)
  uint16_t to_ds: 1;            // К сети (To Distribution System)
  uint16_t from_ds: 1;          // От сети (From Distribution System)
  uint16_t more_frag: 1;        // Есть еще фрагменты
  uint16_t retry: 1;            // Повторная отправка
  uint16_t power_mgmt: 1;       // Управление питанием
  uint16_t more_data: 1;        // Есть больше данных для клиента
  uint16_t protected_frame: 1;  // Кадр зашифрован
  uint16_t order: 1;            // Упорядоченная передача
} wifi_ieee80211_frame_ctrl_t;
typedef struct __attribute__((packed)) {
  wifi_ieee80211_frame_ctrl_t frame_ctrl; // Управление кадром
  uint16_t duration_id;         // Продолжительность / ID
  uint8_t addr1[6];             // Адрес 1 (Получатель)
  uint8_t addr2[6];             // Адрес 2 (Отправитель)
  uint8_t addr3[6];             // Адрес 3 (BSSID / Фильтрация)
  uint16_t seq_ctrl;            // Контроль последовательности
  uint8_t addr4[6];             // Адрес 4 (Опционально)
} wifi_ieee80211_mac_hdr_t;
typedef struct __attribute__((packed)) {
  wifi_ieee80211_mac_hdr_t hdr; // Заголовок
  uint8_t payload[0];           // Полезная нагрузка / Данные
} wifi_ieee80211_packet_t;

// Функции
void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  // return ESP_OK;
}
void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type) {
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff; // Преобразование буфера в структуру promiscuous пакета ESP
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload; // Извлечение заголовка MAC 802.11 из поля payload (полезной нагрузки)
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr; // Получение указателя на заголовок MAC для удобного доступа

    detectedPacketsTemp++;      // Увеличение общего счетчика захваченных пакетов
  
    if (type == WIFI_PKT_MGMT) { // Проверка, является ли тип пакета управляющим кадром (Management Frame)
        if (hdr->frame_ctrl.subtype == 12 || hdr->frame_ctrl.subtype == 10) { // Проверка подтипов 12 (Deauthentication) или 10 (Disassociation)
            detectedPacketsDeauth++; // Увеличение счетчика пакетов Deauth/Disassoc
        } 
        else if (hdr->frame_ctrl.subtype == 8) { // Проверка подтипа 8 (Beacon Frame)
            detectedPacketsBeacon++; // Увеличение счетчика Beacon пакетов
        }
    }
}

void setupPacketMonitor() { // Инициализация монитора и очистка истории
  WiFi.disconnect();
  // tcpip_adapter_init();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  // ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
  // ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
  ESP_ERROR_CHECK(esp_wifi_start());
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
  esp_wifi_set_channel(packetMonitorChannel, WIFI_SECOND_CHAN_NONE);

  for (int i = 0; i < 128; i++) {
    detectedPacketsHistory[i] = 0;
  }
}
void teardownPacketMonitor() { // Отключение монитора
  esp_wifi_set_promiscuous(false); 
  esp_wifi_set_promiscuous_rx_cb(NULL);

  ESP_ERROR_CHECK(esp_wifi_stop());
  ESP_ERROR_CHECK(esp_wifi_deinit());

  // esp_event_loop_delete_default(); 

  WiFi.disconnect();
}
void handlePacketMonitor() { // Поддерживает работу монитора
  esp_wifi_set_channel(packetMonitorChannel, WIFI_SECOND_CHAN_NONE);

  detectedPacketsPerSecond = detectedPacketsTemp;  
  detectedPacketsTotal += detectedPacketsTemp;  
  detectedPacketsTemp = 0;

  for (int i = 0; i < 128 - 1; i++) {  
    detectedPacketsHistory[i] = detectedPacketsHistory[i + 1];
  }
  detectedPacketsHistory[128 - 1] = detectedPacketsPerSecond;  
}
void drawPacketMonitorMenu(U8G2& u8g2) { // Отрисовка меню монитора
  u8g2.setFont(UI_FONT); // Установка шрифта
  char buf[4]; // Буфер для текста

  for (int i = 0; i < 128; i++) { // Цикл отрисовки линий
    int y = constrain(map(detectedPacketsHistory[i], 0, 100, 64, 16), 16, 64); // Изменение Y координаты
    u8g2.drawLine(i, 64, i, y); // Отрисовка линии
  }

  u8g2.drawXBM(95, 3, 8, 8, bitmap_small_skull);        // Deauth Icon
  u8g2.drawXBM(62, 3, 8, 8, bitmap_small_wifi);         // Beacon Icon
  u8g2.drawXBM(29, 3, 8, 8, bitmap_small_packet_monitor); // Total Icon
  u8g2.drawXBM(3, 3, 8, 8, bitmap_small_arrows);        // Channel Icon

  sprintf(buf, "%03d", constrain(detectedPacketsDeauth, 0, 999));
  u8g2.drawStr(107, 11, buf);  // Deauth пакеты

  sprintf(buf, "%03d", constrain(detectedPacketsBeacon, 0, 999));
  u8g2.drawStr(74, 11, buf);   // Beacon пакеты

  sprintf(buf, "%03d", constrain(detectedPacketsTotal, 0, 999));
  u8g2.drawStr(41, 11, buf);   // Total пакеты

  sprintf(buf, "%02d", constrain(packetMonitorChannel, 1, 14));
  u8g2.drawStr(14, 11, buf);   // Канал
}
