#include <SHT3x.h>

SHT3x Sensor;

#include secret_pass.h

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


const int pumpPin = 12;

bool autoControl = true;
float desiredTemperature = 25.0;
float desiredHumidity = 60.0;

unsigned long lastSprayTime = 0;
unsigned long lastCheckTime = 0;

void setup() {
  pinMode(pumpPin, OUTPUT);
  Serial.begin(9600);
  Sensor.Begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void getTemperatureAndHumidity(float& temperature, float& humidity) {
  Sensor.UpdateData();
  temperature = Sensor.GetTemperature();
  humidity = Sensor.GetRelHumidity();
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
}

void autoControlMode(float& temperature, float& humidity) {
  float currentTemperature, currentHumidity;
  getTemperatureAndHumidity(currentTemperature, currentHumidity);

  unsigned long currentMillis = millis();
  
  if (currentTemperature > temperature || currentHumidity < humidity) {
    if (currentMillis - lastSprayTime >= 10000) {
      digitalWrite(pumpPin, HIGH);
      Blynk.setProperty(V3, "color", "#2EA5D8");
      Blynk.virtualWrite(V3, 1); 
      lastSprayTime = currentMillis; 
      delay(2000);
      digitalWrite(pumpPin, LOW);
      Blynk.setProperty(V3, "color", "#FF0000" );
      Blynk.virtualWrite(V3, 0); 
    }
  } else {
    digitalWrite(pumpPin, LOW);
    Blynk.setProperty(V3, "color", "#FF0000" );
    Blynk.virtualWrite(V3, 0); 
  }
}

void loop() {
  Blynk.run();
  Blynk.setProperty(V3, "label", "Máy bơm");
  Blynk.setProperty(V1, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V2, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V4, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V5, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V6, "color", autoControl ? "#2EA5D8" : "#FF0000");

  if (!autoControl) {
    BLYNK_WRITE(V3);
  }

  if (autoControl) {
    autoControlMode(desiredTemperature, desiredHumidity);
  }
}

BLYNK_WRITE(V4) {
  autoControl = param.asInt();
}

BLYNK_WRITE(V3) {
  if (!autoControl) {
    int pumpState = param.asInt();
    digitalWrite(pumpPin, pumpState);
    float temperature = Sensor.GetTemperature();
    float humidity = Sensor.GetRelHumidity();
    Blynk.virtualWrite(V1, temperature);
    Blynk.virtualWrite(V2, humidity);
    Blynk.setProperty(V3, "color", pumpState == HIGH ? "#00FF00" : "#FF0000");
  }
}

BLYNK_WRITE(V5) {
  desiredTemperature = param.asFloat();
}

BLYNK_WRITE(V6) {
  desiredHumidity = param.asFloat();
}
