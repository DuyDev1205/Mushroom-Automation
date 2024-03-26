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
  if (autoControl) {
    // Nếu chế độ tự động được kích hoạt, đặt màu xanh cho tất cả các nút
    Blynk.setProperty(V1, "color", "#2EA5D8"); // Màu xanh
    Blynk.setProperty(V2, "color", "#2EA5D8"); // Màu xanh
    Blynk.setProperty(V4, "color", "#2EA5D8"); // Màu xanh
    Blynk.setProperty(V5, "color", "#2EA5D8"); // Màu xanh
    Blynk.setProperty(V6, "color", "#2EA5D8"); // Màu xanh
  } else {
    // Nếu chế độ tự động không được kích hoạt, đặt màu đỏ cho tất cả các nút
    Blynk.setProperty(V1, "color", "#FF0000"); // Màu đỏ
    Blynk.setProperty(V2, "color", "#FF0000"); // Màu đỏ
    Blynk.setProperty(V4, "color", "#FF0000"); // Màu đỏ
    Blynk.setProperty(V5, "color", "#FF0000"); // Màu đỏ
    Blynk.setProperty(V6, "color", "#FF0000"); // Màu đỏ
  }
}

void manageAutoControl() {
  updateBlynkUI();
  if (autoControl) {
    autoControlMode(desiredTemperature, desiredHumidity);
  }
}

void autoControlMode(float& temperature, float& humidity) {
  Sensor.UpdateData(); // Cập nhật dữ liệu từ cảm biến SHT
  float currentHumidity = Sensor.GetRelHumidity(); // Đọc độ ẩm từ cảm biến SHT
  unsigned long currentMillis = millis();

  if (currentHumidity < humidity && currentMillis - lastSprayTime >= 10000) {
    digitalWrite(pumpPin, HIGH);
    Blynk.setProperty(V3, "color", "#2EA5D8");
    Blynk.virtualWrite(V3, 1);
    lastSprayTime = currentMillis;
    delay(2000);
    digitalWrite(pumpPin, LOW);
    Blynk.setProperty(V3, "color", "#FF0000");
    Blynk.virtualWrite(V3, 0);
  } else {
    digitalWrite(pumpPin, LOW);
    Blynk.setProperty(V3, "color", "#FF0000");
    Blynk.virtualWrite(V3, 0);
  }
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
