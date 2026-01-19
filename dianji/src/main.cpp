#include <Arduino.h>
#include <AccelStepper.h> //调用电机有关库

#define en_x 6     // 电机使能
#define xdirPin 4  // 电机左-方向
#define xstepPin 5 // 电机左-脉冲

AccelStepper stepper(1, xstepPin, xdirPin, en_x); // 建立步进电机对象

void setup()
{
  pinMode(en_x, OUTPUT);
  pinMode(xdirPin, OUTPUT);
  pinMode(xstepPin, OUTPUT);

  stepper.setMaxSpeed(4000.0);    // 设置电机最大速度
  stepper.setAcceleration(200.0); // 设置电机加速度

  stepper.setCurrentPosition(0); // 设置当前位置为电机零位
}

void loop()
{
  // 相对移动：顺时针2048步
  Serial.println("移动 2048 步 (顺时针)");
  stepper.move(2048);
  stepper.runToPosition(); // 阻塞执行，直到电机到达目标位置
  delay(1000);

  // 绝对位置移动：移动到位置1000
  Serial.println("移动到绝对位置 1000");
  stepper.moveTo(1000);
  stepper.runToPosition();
  delay(1000);
}