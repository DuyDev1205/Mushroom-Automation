#include "secret_pass.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// #include <SHT3x.h> // Loại bỏ thư viện cảm biến SHT
// SHT3x Sensor; // Khai báo cảm biến SHT

const int pumpPin = 12;
bool autoControl = true;
float desiredTemperature = 25.0;
float desiredHumidity = 60.0;
unsigned long lastSprayTime = 0;

void setup() {
  pinMode(pumpPin, OUTPUT);
  Serial.begin(9600);

  connectToWiFi(); // Kết nối WiFi

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  updateBlynkUI();
  // Sensor.Begin(); // Khởi động cảm biến SHT
  Blynk.syncVirtual(V6);
  Blynk.syncVirtual(V4);
}

void loop() {
  Blynk.run(); // Cập nhật giao diện người dùng Blynk
  manageAutoControl(); // Quản lý chế độ tự động
  while (WiFi.status() != WL_CONNECTED) {
    connectToWiFi(); 
    delay(1000); 
  }

  // Sensor.UpdateData(); // Cập nhật dữ liệu từ cảm biến SHT
  // float temperature = Sensor.GetTemperature(); // Đọc nhiệt độ từ cảm biến SHT
  // float humidity = Sensor.GetRelHumidity(); // Đọc độ ẩm từ cảm biến SHT

  // Serial.print("Độ ẩm: ");
  // Serial.print(humidity);
  // Serial.print("% - Nhiệt độ: ");
  // Serial.print(temperature);
  // Serial.println("°C");

  // Blynk.virtualWrite(V1, temperature); // Gửi dữ liệu nhiệt độ đến Blynk
  // Blynk.virtualWrite(V2, humidity); // Gửi dữ liệu độ ẩm đến Blynk
}

void connectToWiFi() {
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Đang kết nối WiFi...");
  }
  Serial.println("Kết nối WiFi thành công");
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
  // Sensor.UpdateData(); // Cập nhật dữ liệu từ cảm biến SHT
  // float currentHumidity = Sensor.GetRelHumidity(); // Đọc độ ẩm từ cảm biến SHT
  unsigned long currentMillis = millis();

  // if (currentHumidity < humidity && currentMillis - lastSprayTime >= 10000) {
  //   digitalWrite(pumpPin, HIGH);
  //   Blynk.setProperty(V3, "color", "#2EA5D8");
  //   Blynk.virtualWrite(V3, 1);
  //   lastSprayTime = currentMillis;
  //   delay(2000);
  //   digitalWrite(pumpPin, LOW);
  //   Blynk.setProperty(V3, "color", "#FF0000");
  //   Blynk.virtualWrite(V3, 0);
  // } else {
  //   digitalWrite(pumpPin, LOW);
  //   Blynk.setProperty(V3, "color", "#FF0000");
  //   Blynk.virtualWrite(V3, 0);
  // }
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
