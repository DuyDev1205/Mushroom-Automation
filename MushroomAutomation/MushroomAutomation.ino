// #include <Blynk.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "secret_pass.h"
#include <SHT3x.h>
SHT3x Sensor;
const char *ssid = SECRET_SSID;//Thay thể tên wifi trong tệp secret_pass.h
const char *password = SECRET_PASS;//Thay thế mật khẩu trong tệp secret_pass.h
// Địa chỉ IP của máy chủ NTP
const char *ntpServer = "pool.ntp.org";

// reagion blynk
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID      "TMPL6r0fdPEst"
#define BLYNK_TEMPLATE_NAME    "Mushroom"
#define BLYNK_AUTH_TOKEN       "zBHRmPs-55II61f8lNHAkv9H3Q4ACr0C"
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
// endreagion blynk

// Cổng NTP
const int ntpPort = 123;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, ntpPort);

void setup() {
  Serial.begin(115200);
  Sensor.Begin();
  // Kết nối WiFi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  timeClient.setTimeOffset(7 * 3600);
  // Bắt đầu đồng bộ thời gian từ máy chủ NTP
  timeClient.begin();
}

void loop() {
   Blynk.run();
  // Cập nhật thời gian
  timeClient.update();

  // Lấy thông tin ngày, tháng, năm, giờ, phút, giây
  String formattedTime = timeClient.getFormattedTime();
  Serial.print(formattedTime+' ');
  Sensor.UpdateData();
  Serial.print("Temperature: ");
  Serial.print(Sensor.GetTemperature());
  Serial.write("\xC2\xB0");
  Serial.print("C\t");
  Serial.print("Humidity: ");
  Serial.print(Sensor.GetRelHumidity());
  Serial.println("%");
  delay(1000); // Đợi 1 giây trước khi lấy thời gian mới
}
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // Đọc trạng thái nút nhấn ảo từ pin ảo V1

  if (pinValue == 0) {
    ledState = HIGH; // Bật đèn
    Blynk.virtualWrite(V0, 0);
  } else {
    ledState = LOW; // Tắt đèn
    Blynk.virtualWrite(V0, 1);
  }
  digitalWrite(ledPin, ledState); // Cập nhật trạng thái đèn
  Serial.println(ledState);
  
}