#include <BlynkSimpleEsp32.h>
#include <SHT3x.h>

SHT3x Sensor;

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

void sendTemperatureAndHumidityData() {
  float temperature = Sensor.GetTemperature();
  float humidity = Sensor.GetRelHumidity();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.write("\xC2\xB0C\t");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
}

void autoControlMode() {
  float temperature = Blynk.virtualRead(V5).asFloat(); // Đọc nhiệt độ mong muốn từ Blynk
  float humidity = Blynk.virtualRead(V6).asFloat(); // Đọc độ ẩm mong muốn từ Blynk
  float currentTemperature = Sensor.GetTemperature();
  float currentHumidity = Sensor.GetRelHumidity();

  unsigned long currentMillis = millis(); // Lấy thời gian hiện tại

  // Kiểm tra xem đã đến lúc kiểm tra nhiệt độ và độ ẩm hay chưa
  if (currentMillis - lastCheckTime >= 10000) {
    // Cập nhật thời gian cuối cùng kiểm tra
    lastCheckTime = currentMillis;
    // Cập nhật dữ liệu nhiệt độ và độ ẩm từ cảm biến
    Sensor.UpdateData();
    // Đọc lại nhiệt độ và độ ẩm hiện tại
    currentTemperature = Sensor.GetTemperature();
    currentHumidity = Sensor.GetRelHumidity();
  }

  // Kiểm tra và điều chỉnh máy bơm dựa trên nhiệt độ và độ ẩm
  if (currentTemperature < temperature) {
    // Nếu nhiệt độ thấp hơn nhiệt độ mong muốn, và đã đủ thời gian từ lần phun sương trước
    if (currentMillis - lastSprayTime >= 10000) {
      digitalWrite(pumpPin, HIGH); // Bật máy bơm
      Blynk.setProperty(V3, "color", "#2EA5D8"); // Màu đỏ hoặc xanh nước biển cho V3 (chức năng bơm)
      lastSprayTime = currentMillis; // Cập nhật thời gian cuối cùng phun sương
    }
  } else if (currentTemperature > temperature + 1) {
    digitalWrite(pumpPin, LOW); // Tắt máy bơm
    Blynk.setProperty(V3, "color", "#FF0000" ); // Màu đỏ hoặc xanh nước biển cho V3 (chức năng bơm)
  }

  if (currentHumidity < humidity) {
    // Nếu độ ẩm thấp hơn độ ẩm mong muốn, và đã đủ thời gian từ lần phun sương trước
    if (currentMillis - lastSprayTime >= 10000) {
      digitalWrite(pumpPin, HIGH); // Bật máy bơm
      Blynk.setProperty(V3, "color", "#2EA5D8");
      lastSprayTime = currentMillis; // Cập nhật thời gian cuối cùng phun sương
    }
  } else if (currentHumidity > humidity + 5) {
    digitalWrite(pumpPin, LOW); // Tắt máy bơm
    Blynk.setProperty(V3, "color", "#FF0000" );
  }
  
}

void loop()
{
  Blynk.run();
  // Cập nhật màu sắc của các nút trên ứng dụng Blynk
  Blynk.setProperty(V1, "color", autoControl ? "#2EA5D8" : "#FF0000"); // Màu xanh nước biển hoặc đỏ cho V1 (nhiệt độ)
  Blynk.setProperty(V2, "color", autoControl ? "#2EA5D8" : "#FF0000"); // Màu xanh nước biển hoặc đỏ cho V2 (độ ẩm)
  Blynk.setProperty(V4, "color", autoControl ? "#2EA5D8" : "#FF0000"); // Màu xanh nước biển hoặc đỏ cho V4 (chức năng tự động)
  Blynk.setProperty(V5, "color", autoControl ? "#2EA5D8" : "#FF0000"); // Màu xanh nước biển hoặc đỏ cho V5 (nhiệt độ mong muốn)
  Blynk.setProperty(V6, "color", autoControl ? "#2EA5D8" : "#FF0000"); // Màu xanh nước biển hoặc đỏ cho V6 (độ ẩm mong muốn)
  
  if(autoControl) {
    autoControlMode();
    
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
    
    // Thay đổi màu của nút tương ứng với trạng thái máy bơm
    Blynk.setProperty(V3, "color", pumpState == HIGH ? "#00FF00" : "#FF0000"); // Màu xanh khi máy bơm được bật, đỏ khi tắt
  }
}
