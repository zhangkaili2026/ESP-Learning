#include <Arduino.h>

// 定义光照采样引脚
#define LIGHT_ADC_PIN 35
// 定义D3灯PWM引脚
#define LED_D3_PIN 3
// LEDC通道与参数配置
#define LEDC_CHANNEL 1    // LEDC通道1
#define LEDC_FREQ 128     // PWM频率128Hz
#define LEDC_RESOLUTION 12// 计数位数12（占空比范围0-4095）
// 采样间隔（0.1秒=100毫秒）
#define SAMPLING_INTERVAL 100

// 上一次采样的时间戳
unsigned long lastSampleTime = 0;
// ADC最大采样值（12位分辨率）
const int ADC_MAX = pow(2, LEDC_RESOLUTION) - 1;

void setup() {
  // 初始化ADC引脚为输入
  pinMode(LIGHT_ADC_PIN, INPUT);
  // 配置ADC分辨率为12位
  analogReadResolution(12);
  // 配置ADC衰减，支持0-3.3V输入
  analogSetAttenuation(ADC_11db);

  // 初始化LEDC PWM
  ledcSetup(LEDC_CHANNEL, LEDC_FREQ, LEDC_RESOLUTION);
  // 将LEDC通道绑定到D3灯引脚
  ledcAttachPin(LED_D3_PIN, LEDC_CHANNEL);
}

void loop() {
  // 每隔0.1秒执行一次采样与PWM更新
  if (millis() - lastSampleTime >= SAMPLING_INTERVAL) {
    lastSampleTime = millis();

    // 1. 读取光照传感器的ADC采样值
    int lightAdcValue = analogRead(LIGHT_ADC_PIN);
    
    // 2. 计算反向占空比：光照越强（ADC值越大），占空比越小，灯光越暗
    int pwmDuty = ADC_MAX - lightAdcValue;
    // 限制占空比在0~ADC_MAX范围内，避免溢出
    pwmDuty = constrain(pwmDuty, 0, ADC_MAX);

    // 3. 更新D3灯的PWM占空比
    ledcWrite(LEDC_CHANNEL, pwmDuty);
  }
}