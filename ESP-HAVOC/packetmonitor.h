// Credits: spacehuhn/PacketMonitor32

#pragma once

// Библиотеки
#include <Arduino.h>
#include <U8g2lib.h>

// Библиотеки ESP32
#include <esp_wifi.h>
#include <esp_netif.h>
#include <esp_event.h>
#include <esp_system.h>
#include <esp_event_loop.h>
// #include <esp_wifi_types.h>
// #include <esp_private/wifi.h>

// Переменные
uint32_t detectedPacketsHistory[128];
uint32_t detectedPacketsDeauth = 0;
uint32_t detectedPacketsBeacon = 0;
uint32_t detectedPacketsTotal = 0;
uint32_t detectedPacketsTemp = 0;
unsigned long detectedPacketsPerSecond = 0;
unsigned long lastPacketMonitorTime = 0;

typedef struct {
    uint16_t proto: 2;
    uint16_t type: 2;
    uint16_t subtype: 4;
    uint16_t to_ds: 1;
    uint16_t from_ds: 1;
    uint16_t more_frag: 1;
    uint16_t retry: 1;
    uint16_t power_mgmt: 1;
    uint16_t more_data: 1;
    uint16_t protected_frame: 1;
    uint16_t order: 1;
} wifi_ieee80211_frame_ctrl_t;
typedef struct __attribute__((packed)) {
  wifi_ieee80211_frame_ctrl_t frame_ctrl;
  uint16_t duration_id;
  uint8_t addr1[6];
  uint8_t addr2[6];
  uint8_t addr3[6];
  uint16_t seq_ctrl;
  uint8_t addr4[6]; 
} wifi_ieee80211_mac_hdr_t;
typedef struct __attribute__((packed)) {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0];
} wifi_ieee80211_packet_t;

void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  // return ESP_OK;
}
void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type) {
  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

  detectedPacketsTemp++;
  
  if (type == WIFI_PKT_MGMT) {
    
    if (hdr->frame_ctrl.subtype == 12 || hdr->frame_ctrl.subtype == 10) {
      detectedPacketsDeauth++;
    } 
    
    else if (hdr->frame_ctrl.subtype == 8) {
      detectedPacketsBeacon++; 
    }
  }
}

void setupPacketMonitor(int channel) {
  // tcpip_adapter_init();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  // ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
  ESP_ERROR_CHECK(esp_wifi_start());
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  for (int i = 0; i < 128; i++) {
    detectedPacketsHistory[i] = 0;
  }
}
void teardownPacketMonitor() {
  esp_wifi_set_promiscuous(false); 
  esp_wifi_set_promiscuous_rx_cb(NULL);

  ESP_ERROR_CHECK(esp_wifi_stop());
  ESP_ERROR_CHECK(esp_wifi_deinit());

  esp_event_loop_delete_default(); 
}
void updateDetectedPacketsHistory() {
  detectedPacketsPerSecond = detectedPacketsTemp;  
  detectedPacketsTotal += detectedPacketsTemp;  
  detectedPacketsTemp = 0;

  for (int i = 0; i < 128 - 1; i++) {  
    detectedPacketsHistory[i] = detectedPacketsHistory[i + 1];
  }
  detectedPacketsHistory[128 - 1] = detectedPacketsPerSecond;  
}