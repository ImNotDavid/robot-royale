#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

const int potPin1 = 32;
const int potPin2 = 34;

typedef struct struct_message {
  int pot1;
  int pot2;
} struct_message;

struct_message outgoingData;

uint8_t broadcastAddress[] = {0xA0, 0x85, 0xE3, 0x4A, 0x90, 0xC4}; // change

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Sender");

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  outgoingData.pot1 = analogRead(potPin1);
  outgoingData.pot2 = analogRead(potPin2);

  esp_now_send(broadcastAddress, (uint8_t *)&outgoingData, sizeof(outgoingData));

  Serial.print("Sending Pot1: ");
  Serial.print(outgoingData.pot1);
  Serial.print(" | Pot2: ");
  Serial.println(outgoingData.pot2);

  delay(200);
}

