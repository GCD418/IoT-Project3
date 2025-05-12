#pragma once
#include <WiFi.h>
class WifiManager
{
private:
  const char *ssid;
  const char *password;

public:
  WifiManager(const char *ssid, const char *password)
  : ssid(ssid), password(password) {}
  void connect()
  {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected to WiFi. IP address: ");
    Serial.println(WiFi.localIP());
  }
  void keepConnection()
  {
    if(WiFi.status() != WL_CONNECTED)
      connect();
  }
};
