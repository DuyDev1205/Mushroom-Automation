#include "secret_pass.h"
#include <WiFiManager.h> // Thêm khai báo thư viện WiFiManager
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
  
  Blynk.begin(BLYNK_AUTH_TOKEN, WiFi.SSID().c_str(), WiFi.psk().c_str());
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
  // Khởi tạo WiFiManager
  WiFiManager wifiManager;

  // Kiểm tra xem ESP8266 có kết nối WiFi hay không
  if (!WiFi.isConnected()) {
    // Thử kết nối WiFi hoặc chuyển sang chế độ điểm truy cập (AP) để cấu hình WiFi mới
    if (!wifiManager.autoConnect("ESP8266_AP")) {
      Serial.println("Failed to connect and hit timeout");
      // Nếu kết nối thất bại sau một khoảng thời gian, reset thiết bị
      ESP.reset();
      delay(1000);
    } else {
      // In ra thông báo khi kết nối WiFi thành công
      Serial.println("Connected to WiFi");
      Serial.print("SSID: ");
      Serial.println(WiFi.SSID()); // In ra tên của mạng WiFi đã kết nối

      autoControl = true;
      wifiConnected = true; // Cập nhật trạng thái kết nối WiFi
    }
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
