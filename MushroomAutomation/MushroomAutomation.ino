
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID      "TMPL6r0fdPEst"
#define BLYNK_TEMPLATE_NAME    "Mushroom"
#define BLYNK_AUTH_TOKEN       "zBHRmPs-55II61f8lNHAkv9H3Q4ACr0C"

#include <Wire.h>
#include <Adafruit_SHT31.h>
Adafruit_SHT31 sht31 = Adafruit_SHT31();

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
#include "secret_pass.h"
// const char *ntpServer = "pool.ntp.org";
// const int ntpPort = 123;
// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, ntpServer, ntpPort);

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timeClient.begin();
  WiFi.begin(ssid, pass);
}

void loop()
{
  Blynk.run();
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  Serial.println("Formatted Time: " + formattedTime);
  Serial.println("hello");
  float temp = sht31.readTemperature();
   float humidity = sht31.readHumidity();
    if (!isnan(temp) && !isnan(humidity)) {
    Serial.print("Nhiệt độ: ");
    Serial.print(temp);
    Blynk.virtualWrite(V1, temp);
    Serial.print(" °C\tĐộ ẩm: ");
    Serial.print(humidity);
    Blynk.virtualWrite(V2, humidity);
    Serial.println(" %");
    } else {
    Serial.println("Không thể đọc dữ liệu từ mạch SHT30!");
    }
    delay(1000); 
}

