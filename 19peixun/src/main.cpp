#include <Arduino.h>
#include <AccelStepper.h>

// 风扇控制引脚（和你课堂演示的一致）
#define FAN_PIN 18
// 步进电机引脚
#define EN_X 6    // 电机使能
#define X_DIR_PIN 4  // 左电机方向
#define X_STEP_PIN 5 // 左电机脉冲
#define EN_Y 45      // 右电机使能
#define Y_DIR_PIN 47 // 右电机方向
#define Y_STEP_PIN 48// 右电机脉冲

// 修正：脉冲+方向模式只传3个参数，使能引脚单独控制
AccelStepper stepper1(1, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepper2(1, Y_STEP_PIN, Y_DIR_PIN);

void setup() {
  // 初始化风扇引脚
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);

  // 初始化电机使能引脚
  pinMode(EN_X, OUTPUT);
  pinMode(EN_Y, OUTPUT);
  // 关键：拉低使能，让电机解锁
  digitalWrite(EN_X, LOW);
  digitalWrite(EN_Y, LOW);

  // 串口初始化
  Serial.begin(115200);

  // 电机参数设置
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(200.0);
  stepper2.setMaxSpeed(1000.0);
  stepper2.setAcceleration(200.0);
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);
}

void loop() {
  // 风扇按课堂代码逻辑，3秒开关一次
  digitalWrite(FAN_PIN, HIGH);
  delay(3000);
  digitalWrite(FAN_PIN, LOW);
  delay(3000);

  // 电机转动：1号正转2048步，2号反转2048步
  stepper1.move(2048);
  stepper2.move(-2048);
  // 循环调用run()直到电机走完
  while (stepper1.run() || stepper2.run());
}