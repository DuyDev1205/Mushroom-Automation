
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID      "TMPL6r0fdPEst"
#define BLYNK_TEMPLATE_NAME    "Mushroom"
#define BLYNK_AUTH_TOKEN       "zBHRmPs-55II61f8lNHAkv9H3Q4ACr0C"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Thu Tamm";
char pass[] = "thutam1975";

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  Blynk.run();
  Serial.println("hello");
  int humidity = 30;
  Blynk.virtualWrite(V2, humidity);
}

