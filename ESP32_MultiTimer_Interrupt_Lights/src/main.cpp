#include <Arduino.h>
#include <esp32-hal-timer.h>

// 定义灯引脚
#define LED_D3 3
#define LED_D4 4
#define LED_D6 6

// 定义定时器对象（hw_timer_t类型）
hw_timer_t *tim1 = NULL;
hw_timer_t *tim2 = NULL;
hw_timer_t *tim3 = NULL;

// 定时时长（预分频80后计数频率1MHz，1us计数1次）
#define TIM1_INTERVAL_US 100000  // 0.1秒
#define TIM2_INTERVAL_US 1000000 // 1秒
#define TIM3_INTERVAL_US 5000000 // 5秒

// 定时器1中断回调函数：翻转D3灯
void IRAM_ATTR service_tim1() {
  digitalWrite(LED_D3, !digitalRead(LED_D3));
}

// 定时器2中断回调函数：翻转D4灯
void IRAM_ATTR service_tim2() {
  digitalWrite(LED_D4, !digitalRead(LED_D4));
}

// 定时器3中断回调函数：翻转D6灯（非自动重载，需手动重启）
void IRAM_ATTR service_tim3() {
  digitalWrite(LED_D6, !digitalRead(LED_D6));
  // 非自动重载，需手动重新写入计数值并启用闹钟
  timerAlarmWrite(tim3, TIM3_INTERVAL_US, false);
  timerAlarmEnable(tim3);
}

void setup() {
  // 初始化灯引脚为输出模式，初始低电平（灯灭）
  pinMode(LED_D3, OUTPUT);
  pinMode(LED_D4, OUTPUT);
  pinMode(LED_D6, OUTPUT);
  digitalWrite(LED_D3, LOW);
  digitalWrite(LED_D4, LOW);
  digitalWrite(LED_D6, LOW);

  // 1. 初始化定时器1：编号1，预分频80，向上计数
  tim1 = timerBegin(1, 80, true);
  // 绑定中断回调，边沿触发
  timerAttachInterrupt(tim1, &service_tim1, true);
  // 设置定时时长，自动重载（true）
  timerAlarmWrite(tim1, TIM1_INTERVAL_US, true);
  // 启用定时器1中断
  timerAlarmEnable(tim1);

  // 2. 初始化定时器2：编号2，预分频80，向上计数
  tim2 = timerBegin(2, 80, true);
  timerAttachInterrupt(tim2, &service_tim2, true);
  timerAlarmWrite(tim2, TIM2_INTERVAL_US, true); // 自动重载
  timerAlarmEnable(tim2);

  // 3. 初始化定时器3：编号3，预分频80，向上计数
  tim3 = timerBegin(3, 80, true);
  timerAttachInterrupt(tim3, &service_tim3, true);
  timerAlarmWrite(tim3, TIM3_INTERVAL_US, false); // 非自动重载
  timerAlarmEnable(tim3);
}

void loop() {
  // 主循环无需操作，定时器中断自动执行
}