#include <Arduino.h>

// 定义4个LED引脚（适配ESP32-S3，可根据实际接线修改）
const int D3_LED = 14;
const int D4_LED = 27;
const int D5_LED = 26;
const int D6_LED = 33;

// LEDC配置参数
const int LEDC_CH1 = 1;
const int LEDC_CH2 = 2;
const int LEDC_CH3 = 3;
const int LEDC_CH4 = 4;
const int LEDC_FREQ = 128;     // 实训要求128Hz
const int LEDC_RES = 8;        // 实训要求8位计数位
const int FADE_STEP = 5;       // 渐变步长
const int FADE_DELAY = 10;     // 渐变延时
const int HOLD_DELAY = 500;    // 保持状态0.5秒

// 呼吸渐亮函数
void fadeIn(int channel) {
  for (int duty = 0; duty <= 255; duty += FADE_STEP) {
    ledcWrite(channel, duty);
    delay(FADE_DELAY);
  }
}

// 呼吸渐暗函数
void fadeOut(int channel) {
  for (int duty = 255; duty >= 0; duty -= FADE_STEP) {
    ledcWrite(channel, duty);
    delay(FADE_DELAY);
  }
}

void setup() {
  // 初始化LEDC各通道
  ledcSetup(LEDC_CH1, LEDC_FREQ, LEDC_RES);
  ledcSetup(LEDC_CH2, LEDC_FREQ, LEDC_RES);
  ledcSetup(LEDC_CH3, LEDC_FREQ, LEDC_RES);
  ledcSetup(LEDC_CH4, LEDC_FREQ, LEDC_RES);

  // 绑定通道到对应LED引脚
  ledcAttachPin(D3_LED, LEDC_CH1);
  ledcAttachPin(D4_LED, LEDC_CH2);
  ledcAttachPin(D5_LED, LEDC_CH3);
  ledcAttachPin(D6_LED, LEDC_CH4);

  // 初始关闭所有LED
  ledcWrite(LEDC_CH1, 0);
  ledcWrite(LEDC_CH2, 0);
  ledcWrite(LEDC_CH3, 0);
  ledcWrite(LEDC_CH4, 0);
}

void loop() {
  // 依次渐亮D3→D4→D5→D6
  fadeIn(LEDC_CH1);
  fadeIn(LEDC_CH2);
  fadeIn(LEDC_CH3);
  fadeIn(LEDC_CH4);
  delay(HOLD_DELAY); // 保持最亮0.5秒

  // 依次渐暗D3→D4→D5→D6
  fadeOut(LEDC_CH1);
  fadeOut(LEDC_CH2);
  fadeOut(LEDC_CH3);
  fadeOut(LEDC_CH4);
  delay(HOLD_DELAY); // 保持熄灭0.5秒
}