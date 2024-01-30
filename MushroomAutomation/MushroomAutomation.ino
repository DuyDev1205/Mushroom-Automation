
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID      "TMPL6r0fdPEst"
#define BLYNK_TEMPLATE_NAME    "Mushroom"
#define BLYNK_AUTH_TOKEN       "zBHRmPs-55II61f8lNHAkv9H3Q4ACr0C"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <SHT3x.h>
SHT3x Sensor;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Thu Tamm";
char pass[] = "thutam1975";

void setup()
{
  // Debug console
  Serial.begin(9600);
  Sensor.Begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  Blynk.run();
  Sensor.UpdateData();
  Serial.print("Temperature: ");
  Serial.print(Sensor.GetTemperature());
  Blynk.virtualWrite(V1, Sensor.GetTemperature());
  Serial.write("\xC2\xB0");
  Serial.print("C\t");
  Serial.print("Humidity: ");
  Serial.print(Sensor.GetRelHumidity());
  Serial.println("%");
  Blynk.virtualWrite(V2, Sensor.GetRelHumidity());
}

