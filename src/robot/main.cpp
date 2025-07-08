#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#define MOTORA1 3
#define MOTORA2 4
#define MOTORB1 1
#define MOTORB2 2

typedef struct struct_message {
  int pot1;
  int pot2;
} struct_message;

struct_message incomingData;

void setMotorA(int speed) {
  if (speed > 0) {
    analogWrite(MOTORA1, 0);
    analogWrite(MOTORA2, speed);
  } else {
    analogWrite(MOTORA2, 0);
    analogWrite(MOTORA1, -speed);
  }
}

void setMotorB(int speed) {
  if (speed > 0) {
    analogWrite(MOTORB1, 0);
    analogWrite(MOTORB2, speed);
  } else {
    analogWrite(MOTORB2, 0);
    analogWrite(MOTORB1, -speed);
  }
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingDataBytes, int len) {
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));

  Serial.print("Received Pot1: ");
  Serial.print(incomingData.pot1);
  Serial.print(" | Pot2: ");
  Serial.println(incomingData.pot2);

  // Map potentiometer values (0-4095) to PWM (-255 to 255)
  int speedA = map(incomingData.pot1, 0, 4095, -255, 255);
  int speedB = map(incomingData.pot2, 0, 4095, -255, 255);
  if (abs(speedA) < 100) speedA = 0;
  if (abs(speedB) < 100) speedB = 0;

  setMotorA(speedA);
  setMotorB(speedB);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Receiver");

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  pinMode(MOTORA1, OUTPUT);
  pinMode(MOTORA2, OUTPUT);
  pinMode(MOTORB1, OUTPUT);
  pinMode(MOTORB2, OUTPUT);
}

void loop() {
  //Serial.println(WiFi.macAddress());
}