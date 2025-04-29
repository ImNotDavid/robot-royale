#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>

const char *ssid = "Winton";
const char *password = "wintonoverwatch";

const char *websockets_server_host = "192.168.1.88";
const uint16_t websockets_server_port = 8765;

using namespace websockets;

WebsocketsClient client;

#define MOTORA1 3
#define MOTORA2 4
#define MOTORB1 1
#define MOTORB2 2

// put function declarations here:
void setMotorA(int speed)
{
  if (speed > 0)
  {
    analogWrite(MOTORA1, 0);
    analogWrite(MOTORA2, speed);
  }
  else
  {
    analogWrite(MOTORA2, 0);
    analogWrite(MOTORA1, speed*-1);
  }
}

void setMotorB(int speed)
{
  if (speed > 0)
  {
    analogWrite(MOTORB1, 0);
    analogWrite(MOTORB2, speed);
  }
  else
  {
    analogWrite(MOTORB2, 0);
    analogWrite(MOTORB1, speed*-1);
  }
}

void onMessageCallback(WebsocketsMessage message)
{
  String str_message = message.data();
  int idx = str_message.indexOf(',');
  int left = str_message.substring(0,idx).toInt();
  int right = str_message.substring(idx+1).toInt();
  // Serial.print(left);
  // Serial.print(",");
  // Serial.println(right);
  setMotorA(left);
  setMotorB(right);
}

void setup()
{

  Serial.begin(115200);
  // Connect to wifi
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++)
  {
    Serial.print(".");
    delay(1000);
  }

  // Check if connected to wifi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("No Wifi!");
    return;
  }

  Serial.println("Connected to Wifi, Connecting to server.");

  Serial.println("Connected to Wifi, Connecting to server.");
  // try to connect to Websockets server
  bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
  if (connected)
  {
    Serial.println("Connected!");
    client.send("Hello Server");
  }
  else
  {
    Serial.println("Not Connected!");
  }

  // run callback when messages are received
  client.onMessage(onMessageCallback);
  // put your setup code here, to run once:
  pinMode(MOTORA1, OUTPUT);
  pinMode(MOTORA2, OUTPUT);
  pinMode(MOTORB1, OUTPUT);
  pinMode(MOTORB2, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (client.available())
  {
    client.poll();
  }
  delay(50);
}
