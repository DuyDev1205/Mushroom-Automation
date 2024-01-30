//reagion #include
 #include <WiFi.h>
 #include <NTPClient.h>
 #include <WiFiUdp.h>
 #include "secret_pass.h"
 
 //reagion sht30 sensor
  #include <Wire.h>
  #include <Adafruit_SHT31.h>
  Adafruit_SHT31 sht31 = Adafruit_SHT31();
 //endreagion sht30 sensor

  #include <WiFiClient.h>

 const char *ntpServer = "pool.ntp.org";
 const int ntpPort = 123;

 WiFiUDP ntpUDP;
 NTPClient timeClient(ntpUDP, ntpServer, ntpPort);
//endreagion #include

//reagion setup
 void setup() {
  Serial.begin(115200);

 //reagion sht30 sensor
  if (!sht31.begin(0x44)) {  // Địa chỉ I2C của mạch SHT30 là 0x44
    Serial.println("Không thể tìm thấy mạch SHT30. Kiểm tra kết nối!");
    while (100);}
  Serial.println("Mạch SHT30 đã được kết nối!");
 //endreagion sht30 sensor

 //reagion wifi
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");}
  Serial.println("Connected to WiFi");
 //endreagion wifi

  timeClient.begin();
 }
//endreagion setup

//reagion loop
 void loop() {
 //reagion time
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  Serial.println("Formatted Time: " + formattedTime);
  delay(1000); 
 //endreagion time

 //reagion code
  //reagion code SHT30
   float temp = sht31.readTemperature();
   float humidity = sht31.readHumidity();
    if (!isnan(temp) && !isnan(humidity)) {
    Serial.print("Nhiệt độ: ");
    Serial.print(temp);
    Serial.print(" °C\tĐộ ẩm: ");
    Serial.print(humidity);
    Serial.println(" %");
    } else {
    Serial.println("Không thể đọc dữ liệu từ mạch SHT30!");
    }
    delay(1000); 
   
  //endreagion code SHT30
 //endreagion code

 }
//endreagion loop
