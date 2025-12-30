#include <Arduino.h>

// 定义采样引脚（对应题目GPIO35）
#define LIGHT_PIN 35
// ADC采样分辨率（12位，取值0-4095）
#define ADC_RESOLUTION 12
// 参考电压（ESP32-S3 ADC默认3.3V）
#define REF_VOLTAGE 3.3f
// 采样间隔（0.5秒=500毫秒）
#define SAMPLING_INTERVAL 500

// 上一次采样的时间戳
unsigned long lastSampleTime = 0;

void setup() {
  // 初始化UART0串口，波特率115200
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // 等待串口就绪
  }

  // 配置ADC引脚和分辨率
  pinMode(LIGHT_PIN, INPUT);
  analogReadResolution(ADC_RESOLUTION); // 设置ADC为12位分辨率
  analogSetAttenuation(ADC_11db); // 配置ADC衰减，支持0-3.3V输入
}

void loop() {
  // 每隔0.5秒执行一次采样
  if (millis() - lastSampleTime >= SAMPLING_INTERVAL) {
    lastSampleTime = millis(); // 更新时间戳

    // 1. 读取ADC采样值
    int adcValue = analogRead(LIGHT_PIN);

    // 2. 换算成实际电压：电压 = 采样值 * 参考电压 / 最大采样值(4095)
    float voltage = adcValue * REF_VOLTAGE / (pow(2, ADC_RESOLUTION) - 1);

    // 3. 串口输出指定格式的字符串，保留2位小数
    Serial.printf("采样值：%d，电压值：%.2f V\n", adcValue, voltage);
  }
}