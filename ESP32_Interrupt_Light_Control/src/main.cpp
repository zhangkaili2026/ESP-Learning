#include <Arduino.h>

// 定义引脚
#define WAVE_PIN 19    // 输出方波的GPIO19
#define INT_PIN 5      // 外部中断的GPIO5
#define LED_D3 3       // D3灯的GPIO3

// 全局变量：中断计数
volatile int num = 0;  // volatile确保变量在中断中被正确修改

// 中断回调函数（中断服务函数）
void IRAM_ATTR service_exit5() {
  num++;  // 中断次数累加
  // 串口打印中断次数（中断中串口打印仅作演示，实际建议减少中断内耗时操作）
  Serial.printf("The count of Exit5: %d\n", num);
  // 翻转D3灯状态
  digitalWrite(LED_D3, !digitalRead(LED_D3));
}

void setup() {
  // 初始化UART0串口，波特率115200
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // 等待串口就绪
  }

  // 初始化方波输出引脚为输出模式
  pinMode(WAVE_PIN, OUTPUT);
  // 初始化D3灯引脚为输出模式，初始低电平（灯灭）
  pinMode(LED_D3, OUTPUT);
  digitalWrite(LED_D3, LOW);

  // 核心修改：中断触发方式改为电平改变（CHANGE）
  attachInterrupt(digitalPinToInterrupt(INT_PIN), service_exit5, CHANGE);

  Serial.println("程序启动，外部中断配置为电平改变触发");
}

void loop() {
  // GPIO19输出2秒周期的方波（1秒高，1秒低）
  digitalWrite(WAVE_PIN, HIGH);
  delay(1000);
  digitalWrite(WAVE_PIN, LOW);
  delay(1000);
}