#include <Arduino.h>

// 定义D3灯引脚（适配ESP32-S3，可根据实际接线修改）
const int D3_LED = 14;  
// LEDC配置参数
const int LEDC_CHANNEL = 1;    // 实训要求的通道1
const int LEDC_FREQ = 128;     // 实训要求的128Hz频率
const int LEDC_RESOLUTION = 8; // 实训要求的8位计数位数
// 呼吸灯渐变步长和延时（控制渐变速度）
const int FADE_STEP = 5;
const int FADE_DELAY = 10;

void setup() {
  // 初始化LEDC通道
  ledcSetup(LEDC_CHANNEL, LEDC_FREQ, LEDC_RESOLUTION);
  // 将LEDC通道绑定到D3灯引脚
  ledcAttachPin(D3_LED, LEDC_CHANNEL);
}

void loop() {
  // 从暗到亮：占空比0到255（8位最大值）
  for (int duty = 0; duty <= 255; duty += FADE_STEP) {
    ledcWrite(LEDC_CHANNEL, duty);
    delay(FADE_DELAY);
  }
  // 从亮到暗：占空比255到0
  for (int duty = 255; duty >= 0; duty -= FADE_STEP) {
    ledcWrite(LEDC_CHANNEL, duty);
    delay(FADE_DELAY);
  }
}
