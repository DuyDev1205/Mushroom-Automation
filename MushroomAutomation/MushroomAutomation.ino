#include <SHT3x.h>

SHT3x Sensor;




/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID      "TMPL6r0fdPEst"
#define BLYNK_TEMPLATE_NAME    "Mushroom"
#define BLYNK_AUTH_TOKEN       "zBHRmPs-55II61f8lNHAkv9H3Q4ACr0C"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Bu Bill";
char pass[] = "@laphoa2017";

const int pumpPin = 12;

bool autoControl = true; // Biến để kiểm soát chế độ tự động hoặc không tự động
float desiredTemperature = 25.0; // Nhiệt độ mong muốn
float desiredHumidity = 60.0; // Độ ẩm mong muốn

unsigned long lastSprayTime = 0; // Thời gian cuối cùng máy bơm phun sương được kích hoạt
unsigned long lastCheckTime = 0; // Thời gian cuối cùng kiểm tra nhiệt độ và độ ẩm

void setup()
{

  pinMode(pumpPin, OUTPUT);
  Serial.begin(9600);
  Sensor.Begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

}

void getTemperatureAndHumidity(float& temperature, float& humidity) {
  // Đọc dữ liệu từ cảm biến
  Sensor.UpdateData();
  temperature = Sensor.GetTemperature();
  humidity = Sensor.GetRelHumidity();
  // Serial.print("Temperature: ");
  // Serial.print(temperature);
  // Serial.write("\xC2\xB0C\t");
  // Serial.print("Humidity: ");
  // Serial.print(humidity);
  // Serial.println("%");
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  
}




void autoControlMode(float& temperature, float& humidity) {
  float currentTemperature, currentHumidity;
  getTemperatureAndHumidity(currentTemperature, currentHumidity);

  unsigned long currentMillis = millis();
  Serial.print("Hien Tai: ");
  Serial.println(currentTemperature);
  Serial.println(currentHumidity);
  Serial.println("Mong muon");
  Serial.println(temperature);
  Serial.println(humidity);
  
  if (currentTemperature > temperature || currentHumidity < humidity) {
    if (currentMillis - lastSprayTime >= 10000) {
      digitalWrite(pumpPin, HIGH);
      Blynk.setProperty(V3, "color", "#2EA5D8");
      Blynk.virtualWrite(V3, 1); 
      lastSprayTime = currentMillis; 
      Serial.println("Last spraytime tang");
      delay(2000);
      digitalWrite(pumpPin,LOW);
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
  Serial.println("may bom tat");
  Blynk.run();
  Blynk.setProperty(V3,"label","Máy bơm");
  Blynk.setProperty(V1, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V2, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V4, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V5, "color", autoControl ? "#2EA5D8" : "#FF0000");
  Blynk.setProperty(V6, "color", autoControl ? "#2EA5D8" : "#FF0000");

  if (autoControl) {
    autoControlMode(desiredTemperature, desiredHumidity);
  }
}

BLYNK_WRITE(V4) // Chức năng bật/tắt tự động từ Blynk
{
  autoControl = param.asInt();
}


BLYNK_WRITE(V3) // Chức năng bật/tắt máy bơm từ Blynk (chế độ không tự động)
{
  if (!autoControl) {
   
 
    int pumpState = param.asInt();
    digitalWrite(pumpPin, pumpState);
  
    float temperature = Sensor.GetTemperature();
    float humidity = Sensor.GetRelHumidity();
    Blynk.virtualWrite(V1, temperature);
    Blynk.virtualWrite(V2, humidity);
    // Thay đổi màu của nút tương ứng với trạng thái máy bơm
    Blynk.setProperty(V3, "color", pumpState == HIGH ? "#00FF00" : "#FF0000"); // Màu xanh khi máy bơm được bật, đỏ khi tắt
  }

}
BLYNK_WRITE(V5)
{
  desiredTemperature = param.asFloat();
}

BLYNK_WRITE(V6)
{
  desiredHumidity = param.asFloat();
}
