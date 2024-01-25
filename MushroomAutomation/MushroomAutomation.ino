#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "secret_pass.h"
const char *ssid = SECRET_SSID;//Thay thể tên wifi trong tệp secret_pass.h
const char *password = SECRET_PASS;//Thay thế mật khẩu trong tệp secret_pass.h

// Địa chỉ IP của máy chủ NTP
const char *ntpServer = "pool.ntp.org";

// Cổng NTP
const int ntpPort = 123;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, ntpPort);

void setup() {
  Serial.begin(115200);

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Bắt đầu đồng bộ thời gian từ máy chủ NTP
  timeClient.begin();
}

void loop() {
  // Cập nhật thời gian
  timeClient.update();

  // Lấy thông tin ngày, tháng, năm, giờ, phút, giây
  String formattedTime = timeClient.getFormattedTime();
  Serial.println("Formatted Time: " + formattedTime);

  delay(1000); // Đợi 1 giây trước khi lấy thời gian mới
}