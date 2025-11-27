// Credits: Theckeegs/KBeacon-Spammer

#include <WiFi.h>
#include <esp_wifi.h>
#include <vector>

// Configuration variables
uint8_t beaconPacket[128];
uint8_t macAddr[6];

int channels[] = {1, 6, 11};

char randomName[32];
char *randomSSID() {
  const char *charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int len = rand() % 22 + 7;
  for (int i = 0; i < len; ++i) {
      randomName[i] = charset[rand() % strlen(charset)];
  }
  randomName[len] = '\0';
  return randomName;
}

const char* funnySSID[] = {
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
  "Ye Olde Internet"
};

const char* rickrollSSID[] = {
  "01 Never gonna give you up",
  "02 Never gonna let you down",
  "03 Never gonna run around",
  "04 and desert you",
  "05 Never gonna make you cry",
  "06 Never gonna say goodbye",
  "07 Never gonna tell a lie",
  "08 and hurt you"
};

void generateRandomMac(uint8_t* mac) {
  for(int i = 0; i < 6; i++) {
      mac[i] = random(256);
  }
  mac[0] |= 0x02;
  mac[0] &= 0xFE;
}

void createBeaconPacket(const String& ssid) {
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
  
  generateRandomMac(macAddr);
  memcpy(&beaconPacket[10], macAddr, 6);
  memcpy(&beaconPacket[16], macAddr, 6);
  
  beaconPacket[36] = 0x00;
  beaconPacket[37] = ssid.length();
  memcpy(&beaconPacket[38], ssid.c_str(), ssid.length());
}

void handleBeaconSpam(
  const char** SSID,
  const int countSSID
) {
  for (int ch = 0; ch < 3; ch++) {
      esp_wifi_set_channel(channels[ch], WIFI_SECOND_CHAN_NONE);
      
      for (int i = 0; i < countSSID; i++) {
          String ssidString = String(SSID[i]);
          createBeaconPacket(ssidString);
          int packetSize = 38 + ssidString.length();
          
          for (int j = 0; j < countSSID; j++) {
              esp_wifi_80211_tx(WIFI_IF_AP, beaconPacket, packetSize, false);
          }
      }
  }
}

void setupBeaconSpam() {
  randomSeed(esp_random());
  
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  esp_wifi_set_mode(WIFI_MODE_AP);
}