#include <Arduino.h>
#include <DHT.h>

// 定义DHT11相关参数
#define DHT_PIN 4          // DHT11数据引脚GPIO4
#define DHT_TYPE DHT11     // 传感器类型为DHT11
// 采样间隔（0.5秒=500毫秒）
#define SAMPLING_INTERVAL 500

// 创建DHT传感器对象
DHT dht(DHT_PIN, DHT_TYPE);
// 上一次采样的时间戳
unsigned long lastSampleTime = 0;

void setup() {
  // 初始化UART0串口，波特率115200
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // 等待串口就绪
  }

  // 初始化DHT11传感器
  dht.begin();
  Serial.println("DHT11采样程序启动");
}

void loop() {
  // 每隔0.5秒执行一次采样
  if (millis() - lastSampleTime >= SAMPLING_INTERVAL) {
    lastSampleTime = millis();

    // 1. 读取温湿度数据
    float humidity = dht.readHumidity();    // 读取湿度
    float temperature = dht.readTemperature(); // 读取温度（摄氏度）

    // 2. 检查数据是否读取成功
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("读取DHT11数据失败");
      return;
    }

    // 3. 按指定格式串口输出，保留1位小数
    Serial.printf("温度：%.1f，湿度：%.1f\n", temperature, humidity);
  }
}