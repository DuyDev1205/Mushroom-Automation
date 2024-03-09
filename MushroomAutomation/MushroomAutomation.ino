#include <SHT3x.h>
SHT3x Sensor;

#include "secret_pass.h"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

const int pumpPin = 12;
bool autoControl = true;
float desiredHumidity = 60.0;
unsigned long lastSprayTime = 0;
unsigned long lastCheckTime = 0;
float currentTemperature, currentHumidity;
int pumpState = LOW;
int pumpCount = 0;
int lastDayPumpCount = 0;
int lastDay = 0;

void setup() {
  pinMode(pumpPin, OUTPUT);
  Serial.begin(9600);
  Sensor.Begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  // Đọc giá trị từ chân V6 khi chương trình chạy lần đầu
  Blynk.syncVirtual(V6);  
}

void getTemperatureAndHumidity(float& temperature, float& humidity) {
  Sensor.UpdateData();
  temperature = Sensor.GetTemperature();
  humidity = Sensor.GetRelHumidity();
}

void autoControlMode(float& humidity) {
  Sensor.UpdateData();
  temperature = Sensor.GetTemperature();
  humidity = Sensor.GetRelHumidity();

  unsigned long currentMillis = millis();
  
  if (currentHumidity < humidity) {
    if (currentMillis - lastSprayTime >= 10000) {
      digitalWrite(pumpPin, HIGH);
      Blynk.setProperty(V3, "color", "#2EA5D8");
      Blynk.virtualWrite(V3, 1); 
      lastSprayTime = currentMillis;
      pumpCount++;
      Blynk.virtualWrite(V7, pumpCount); // Cập nhật số lần bơm
      lastDayPumpCount++;
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
  Blynk.setProperty(V1, "color", "#2EA5D8");
  Blynk.setProperty(V2, "color", "#2EA5D8");
  Blynk.setProperty(V4, "color", "#2EA5D8");
  Blynk.setProperty(V5, "color", "#2EA5D8");
  Blynk.setProperty(V6, "color", "#2EA5D8");
  Serial.println("desiredhumidity");
  Serial.println(desiredHumidity);
  float temperature, humidity;
  getTemperatureAndHumidity(temperature, humidity);
  Serial.println(temperature);
  Serial.println(humidity);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  Blynk.setProperty(V3, "color", pumpState == HIGH ? "#00FF00" : "#FF0000");

  if (!autoControl) {
    BLYNK_WRITE(V3);
  }
  if (autoControl) {
    autoControlMode(desiredHumidity);
  }

  unsigned long currentTime = millis();
  int currentDay = currentTime / (1000 * 60 * 60 * 24); // Lấy ngày hiện tại
  Serial.println("Ngay hien tai: ");
  Serial.println(currentDay);
  if (currentDay != lastDay) { // Nếu đã qua ngày mới
    lastDay = currentDay;
    lastDayPumpCount = pumpCount; // Lưu trữ số lần bơm của ngày hôm trước
    pumpCount = 0; // Reset số lần bơm
    Blynk.virtualWrite(V8, lastDayPumpCount); // Cập nhật số lần bơm của ngày hôm trước
    Blynk.virtualWrite(V7, pumpCount); // Cập nhật số lần bơm
  }
}

BLYNK_WRITE(V4) {
  autoControl = param.asInt();
}

BLYNK_WRITE(V3) {
    pumpState = param.asInt();
    digitalWrite(pumpPin, pumpState);
}

BLYNK_WRITE(V5) {
  desiredHumidity = param.asFloat();
}
