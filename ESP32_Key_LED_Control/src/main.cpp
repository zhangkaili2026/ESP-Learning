#include <Arduino.h>

// 定义LED和按键引脚（根据ESP32-S3实际接线修改）
const int LED_D3 = 14;  // 对应原实训D3，可改ESP32-S3引脚
const int LED_D4 = 27;  // 对应原实训D4，可改ESP32-S3引脚
const int KEY_SW1 = 32; // 对应原实训SW1，可改ESP32-S3引脚
const int KEY_SW2 = 39; // 对应原实训SW2，可改ESP32-S3引脚

// 存储LED状态
bool ledD3_State = LOW;
bool ledD4_State = LOW;

// 按键扫描函数
void scanKey() {
  // 处理SW1按键，切换D3状态
  if (digitalRead(KEY_SW1) == LOW) { // 按键按下（假设按键为下拉模式，按下为低电平）
    ledD3_State = !ledD3_State;
    digitalWrite(LED_D3, ledD3_State);
    while (digitalRead(KEY_SW1) == LOW); // 等待按键松开
  }

  // 处理SW2按键，切换D4状态
  if (digitalRead(KEY_SW2) == LOW) {
    ledD4_State = !ledD4_State;
    digitalWrite(LED_D4, ledD4_State);
    while (digitalRead(KEY_SW2) == LOW); // 等待按键松开
  }
}

void setup() {
  // 初始化LED引脚为输出
  pinMode(LED_D3, OUTPUT);
  pinMode(LED_D4, OUTPUT);
  // 初始化按键引脚为输入（上拉模式，避免浮空）
  pinMode(KEY_SW1, INPUT_PULLUP);
  pinMode(KEY_SW2, INPUT_PULLUP);
  // 初始关闭LED
  digitalWrite(LED_D3, LOW);
  digitalWrite(LED_D4, LOW);
}

void loop() {
  scanKey(); // 循环扫描按键
  delay(10); // 消抖延时，提升稳定性
}