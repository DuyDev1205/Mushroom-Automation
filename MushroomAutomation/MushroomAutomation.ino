
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();

#include "secret_pass.h"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const int pumpPin = D6; // Chân kết nối của máy bơm với ESP8266

bool autoControl = true;
float desiredTemperature = 25.0;
float desiredHumidity = 60.0;

unsigned long lastSprayTime = 0;
unsigned long lastCheckTime = 0;

void setup() {
  pinMode(pumpPin, OUTPUT);
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
   while (!Serial)
    delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");
    // Đọc giá trị từ chân V6 khi chương trình chạy lần đầu
  Blynk.syncVirtual(V6);
}

void autoControlMode(float& temperature, float& humidity) {
  float t = sht31.readTemperature();
  float currentHumidity = sht31.readHumidity();

  unsigned long currentMillis = millis();
  
  if ( currentHumidity < humidity) {
    if (currentMillis - lastSprayTime >= 10000) {
      digitalWrite(pumpPin, HIGH);
      Blynk.setProperty(V3, "color", "#2EA5D8");
      Blynk.virtualWrite(V3, 1); 
      lastSprayTime = currentMillis; 
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
  Blynk.setProperty(V1, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V2, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V4, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V5, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V6, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Serial.println("desiredhumidity");
  Serial.println(desiredHumidity);
  if (!autoControl) {
    BLYNK_WRITE(V3);
  }

  if (autoControl) {
    autoControlMode(desiredTemperature, desiredHumidity);
  }

  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();


  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("% - Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  Blynk.virtualWrite(V1, temperature); // Gửi dữ liệu nhiệt độ đến ứng dụng Blynk
  Blynk.virtualWrite(V2, humidity); // Gửi dữ liệu độ ẩm đến ứng dụng Blynk

  delay(2000); // Đợi 2 giây trước khi đọc lại dữ liệu từ cảm biến
}

BLYNK_WRITE(V4) {
  autoControl = param.asInt();
}

BLYNK_WRITE(V3) {
  if (!autoControl) {
    int pumpState = param.asInt();
    digitalWrite(pumpPin, pumpState);
    Blynk.setProperty(V3, "color", pumpState == 1 ? "#00FF00" : "#FF0000");
    float temperature = sht31.readTemperature();
    float humidity = sht31.readHumidity();
    Blynk.virtualWrite(V1, temperature);
    Blynk.virtualWrite(V2, humidity);
    
  }
}

BLYNK_WRITE(V5) {
  desiredTemperature = param.asFloat();
}

BLYNK_WRITE(V6) {
  desiredHumidity = param.asFloat();
}
