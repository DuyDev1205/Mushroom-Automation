#include <DHT.h>

#define DHTPIN D2     // Chân kết nối của cảm biến DHT11 với ESP32
#define DHTTYPE DHT11   // Loại cảm biến (DHT11 hoặc DHT22)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float humidity = dht.readHumidity(); // Đọc độ ẩm từ cảm biến
  float temperature = dht.readTemperature(); // Đọc nhiệt độ từ cảm biến

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("% - Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  delay(2000); // Đợi 2 giây trước khi đọc lại dữ liệu từ cảm biến
}
