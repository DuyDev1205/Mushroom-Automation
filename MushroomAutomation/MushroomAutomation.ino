#include "secret_pass.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const int pumpPin = 12;
bool autoControl = true;
float desiredTemperature = 25.0;
float desiredHumidity = 60.0;
unsigned long lastSprayTime = 0;

bool wifiConnected = false; // Biến để theo dõi trạng thái kết nối WiFi

void setup() {
  pinMode(pumpPin, OUTPUT);
  Serial.begin(9600);

  connectToWiFi(); // Kết nối WiFi

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  updateBlynkUI();
  Blynk.syncVirtual(V6);
  Blynk.virtualWrite(V4, 1);
  autoControl = true;
}

void loop() {
  if (!wifiConnected) {
    // Nếu mất kết nối WiFi, thực hiện kết nối lại
    connectToWiFi();
  }

  Blynk.run();
  manageAutoControl();
}

void connectToWiFi() {
  WiFi.begin(ssid, pass);
  unsigned long startTime = millis(); // Thời gian bắt đầu kết nối WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Đang kết nối WiFi...");
    // Nếu quá thời gian kết nối (ví dụ: 30 giây), thoát vòng lặp
    if (millis() - startTime > 30000) {
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true; // Cập nhật trạng thái kết nối WiFi
    Serial.println("Kết nối WiFi thành công");
    // Bật chế độ tự động khi kết nối lại WiFi
    Blynk.virtualWrite(V4, 1);
    autoControl = true;
  } else {
    wifiConnected = false; // Cập nhật trạng thái kết nối WiFi
    Serial.println("Kết nối WiFi không thành công");
    // Tắt tất cả các thiết bị khi mất kết nối WiFi
    digitalWrite(pumpPin, LOW);
    Blynk.virtualWrite(V4, 0);
    autoControl = false;
  }
}

void updateBlynkUI() {
  Blynk.setProperty(V3, "label", "Máy bơm");
  Blynk.setProperty(V1, "color", "#2EA5D8");
  Blynk.setProperty(V2, "color", "#2EA5D8");
  Blynk.setProperty(V4, "color", "#2EA5D8");
  Blynk.setProperty(V5, "color", "#2EA5D8");
  Blynk.setProperty(V6, "color", "#2EA5D8");
}

void manageAutoControl() {
  if (autoControl) {
    autoControlMode(desiredTemperature, desiredHumidity);
  }
}

void autoControlMode(float& temperature, float& humidity) {
  // Thêm code quản lý chế độ tự động ở đây
}

BLYNK_WRITE(V4) {
  autoControl = param.asInt();
}

BLYNK_WRITE(V3) {
  if (!autoControl) {
    int pumpState = param.asInt();
    digitalWrite(pumpPin, pumpState);
    Blynk.setProperty(V3, "color", pumpState == 1 ? "#00FF00" : "#FF0000");
  }
}

BLYNK_WRITE(V5) {
  desiredTemperature = param.asFloat();
}

BLYNK_WRITE(V6) {
  desiredHumidity = param.asFloat();
}
