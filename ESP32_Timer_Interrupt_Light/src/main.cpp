#include <Arduino.h>
#include <esp32-hal-timer.h>

// 定义D3灯引脚
#define LED_D3 3
// 定时器对象（hw_timer_t类型）
hw_timer_t *tim0 = NULL;
// 定时时长2秒（预分频80后计数频率1MHz，2秒=2000000us）
#define TIMER_INTERVAL_US 2000000

// 定时器中断回调函数
void IRAM_ATTR service_tim0() {
  // 翻转D3灯状态
  digitalWrite(LED_D3, !digitalRead(LED_D3));
}

void setup() {
  // 初始化D3灯引脚
  pinMode(LED_D3, OUTPUT);
  digitalWrite(LED_D3, LOW);

  // 1. 初始化定时器0：预分频80，向上计数（true）
  // timerBegin(定时器号, 预分频系数, 向上计数标志)
  tim0 = timerBegin(0, 80, true);

  // 2. 设置定时器中断：绑定回调函数，边沿触发（true）
  // timerAttachInterrupt(定时器对象, 回调函数, 边沿触发标志)
  timerAttachInterrupt(tim0, &service_tim0, true);

  // 3. 配置定时时长：2000000us，自动重载（true）
  // timerAlarmWrite(定时器对象, 计数值, 自动重载标志)
  timerAlarmWrite(tim0, TIMER_INTERVAL_US, true);

  // 4. 使能开启定时器中断
  // timerAlarmEnable(定时器对象)
  timerAlarmEnable(tim0);
}

void loop() {
  // 主循环无需操作，定时器中断自动执行
}