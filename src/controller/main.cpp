#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

const int xPotPin = 4;
const int yPotPin = 3;
int left_speed = 0;
int right_speed = 0;

typedef struct struct_message
{
  int left_speed;
  int right_speed;
} struct_message;

struct_message outgoingData;

uint8_t broadcastAddress[] = {0xA0, 0x85, 0xE3, 0x4A, 0x90, 0xC4}; // change

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  if(status != ESP_NOW_SEND_SUCCESS){
    Serial.print("Last Packet Send Status: Fail");
  }
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Sender");

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}

void joystickToDifferential(int xVal, int yVal, int &leftMotor, int &rightMotor)
{
  // Constants for joystick range (adjust based on your joystick)
  const int MAX_VALUE = 2048;
  const int CENTER = 2331;
  const int DEADZONE = 50;
  const int MAX_PWM = 255; // motor PWM range

  // Normalize joystick input to -512 to 511 range
  int x = xVal - CENTER;
  int y = yVal - CENTER;

  // Apply deadzone
  if (abs(x) < DEADZONE)
    x = 0;
  if (abs(y) < DEADZONE)
    y = 0;

  // Mix X and Y for differential drive
  int left = x - y;
  int right = x + y;

  // Clamp values to -512 to 511
  left = constrain(left, -MAX_VALUE, MAX_VALUE);
  right = constrain(right, -MAX_VALUE, MAX_VALUE);

  // Map to motor PWM range (-255 to 255)
  leftMotor = map(left, -MAX_VALUE, MAX_VALUE, -MAX_PWM, MAX_PWM);
  rightMotor = map(right, -MAX_VALUE, MAX_VALUE, -MAX_PWM, MAX_PWM);
}

void loop()
{
  joystickToDifferential(analogRead(xPotPin),analogRead(yPotPin), outgoingData.left_speed , outgoingData.right_speed);
  Serial.printf("PWM l: \t %i| r: \t %i\n",outgoingData.left_speed,outgoingData.right_speed);

  esp_now_send(broadcastAddress, (uint8_t *)&outgoingData, sizeof(outgoingData));
 


  delay(100);
}
