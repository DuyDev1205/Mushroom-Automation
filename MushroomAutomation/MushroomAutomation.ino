#include "secret_pass.h"

#include <Wire.h>
#include "Adafruit_SHT31.h"
Adafruit_SHT31 sht31 = Adafruit_SHT31();

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const int pumpPin = D6;
bool autoControl = true;
float desiredTemperature = 25.0;
float desiredHumidity = 60.0;
unsigned long lastSprayTime = 0;
bool wifiConnected = false;
bool sensorConnected = false;

void setup() {
  pinMode(pumpPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("SHT31 test");

  connectToWiFi(); // Kết nối WiFi
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  updateBlynkUI();
  Blynk.syncVirtual(V6);
  Blynk.virtualWrite(V4, 1);
  autoControl = true;

  // Thử kết nối cảm biến
  if (sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("SHT31 connected successfully.");
    sensorConnected = true; // Đặt cờ kết nối cảm biến thành công
  } else {
    Serial.println("Couldn't find SHT31. Check wiring!");
    sensorConnected = false;
  }
}

void reconnectSensor() {
  Serial.println("Attempting to reconnect sensor...");
}

void loop() {
  if (!wifiConnected) {
    connectToWiFi();
  }
  if (sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("SHT31 connected successfully.");
    sensorConnected = true; // Đặt cờ kết nối cảm biến thành công
  } else {
    Serial.println("Couldn't find SHT31. Check wiring!");
    sensorConnected = false;
  }
  
  Blynk.run();

  
  Blynk.run();

  if (!sensorConnected) {
    reconnectSensor();
  } else {
    readAndProcessSensorData();
  }
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    if (millis() - startTime > 30000) {
      Serial.println("\nFailed to connect to WiFi");
      wifiConnected = false;
      return;
    }
  }
  Serial.println("\nWiFi connected successfully");
  wifiConnected = true;
  Blynk.virtualWrite(V4, 1); // Re-enable autoControl on reconnect
  autoControl = true;
}

void readAndProcessSensorData() {
  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  Serial.println("hehe");
  manageAutoControl();

}

void updateBlynkUI() {
  if (autoControl) {
    Blynk.setProperty(V1, "color", "#2EA5D8");
    Blynk.setProperty(V2, "color", "#2EA5D8");
    Blynk.setProperty(V4, "color", "#2EA5D8");
    Blynk.setProperty(V5, "color", "#2EA5D8");
    Blynk.setProperty(V6, "color", "#2EA5D8");
  } else {
    Blynk.setProperty(V1, "color", "#FF0000");
    Blynk.setProperty(V2, "color", "#FF0000");
    Blynk.setProperty(V4, "color", "#FF0000");
    Blynk.setProperty(V5, "color", "#FF0000");
    Blynk.setProperty(V6, "color", "#FF0000");
  }
}

void manageAutoControl() {
  updateBlynkUI();
  if (autoControl) {
    autoControlMode(desiredTemperature, desiredHumidity);
  }
}

void autoControlMode(float& temperature, float& humidity) {
  float currentHumidity = sht31.readHumidity();
  unsigned long currentMillis = millis();
  if (currentHumidity < humidity && currentMillis - lastSprayTime >= 10000) {
    digitalWrite(pumpPin, HIGH);
    lastSprayTime = currentMillis;
    delay(2000);
    digitalWrite(pumpPin, LOW);
  }
}

BLYNK_WRITE(V4) {
  autoControl = param.asInt();
}

BLYNK_WRITE(V3) {
  if (!autoControl) {
    int pumpState = param.asInt();
    digitalWrite(pumpPin, pumpState);
  }
}

BLYNK_WRITE(V5) {
  desiredTemperature = param.asFloat();
}

BLYNK_WRITE(V6) {
  desiredHumidity = param.asFloat();
}
