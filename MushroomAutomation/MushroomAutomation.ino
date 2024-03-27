#include "secret_pass.h"

#include <Wire.h>
#include "Adafruit_SHT31.h"
bool enableHeater = false;
uint8_t loopCnt = 0;
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
  while (!sensorConnected) {
    Serial.println("Initializing SHT31...");
    if (sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
      Serial.println("SHT31 connected successfully.");
      sensorConnected = true; // Đặt cờ kết nối cảm biến thành công
    } else {
      Serial.println("Couldn't find SHT31. Retrying...");
      delay(1000);
    }
  }

  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");
}

void reconnectSensor() {
  Serial.println("Attempting to reconnect sensor...");
  
  // Thử khởi tạo lại cảm biến
  if (!sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31. Retrying...");
    delay(1000);
    return; // Thử lại sau một khoảng thời gian
  }

  Serial.println("Sensor reconnected successfully.");
}

void loop() {
  if (!wifiConnected) {
    // Nếu mất kết nối WiFi, thực hiện kết nối lại
    connectToWiFi();
  }

  Blynk.run();

  // Kiểm tra kết nối cảm biến, nếu mất kết nối, reconnect
  if (!sht31.begin(0x44)) {
    reconnectSensor();
  } else {
    // Nếu cảm biến kết nối thành công, tiếp tục thực hiện các thao tác khác
    float temperature = sht31.readTemperature();
    float humidity = sht31.readHumidity();
    Blynk.virtualWrite(V1, temperature);
    Blynk.virtualWrite(V2, humidity);
    Serial.println("hehe");
    manageAutoControl();
  }
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
  
  float currentHumidity = sht31.readHumidity(); // Đọc độ ẩm từ cảm biến SHT
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
