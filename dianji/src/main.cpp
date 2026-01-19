#include <Arduino.h>
#include <AccelStepper.h> //调用电机有关库

#define en_x 6      // 电机使能
#define xdirPin 4   // 电机左-方向
#define xstepPin 5  // 电机左-脉冲
#define en_y 45     // 电机使能
#define ydirPin 47  // 电机右-方向
#define ystepPin 48 // 电机右-脉冲

AccelStepper stepper1(1, xstepPin, xdirPin, en_x); // 建立右边步进电机对象
AccelStepper stepper2(1, ystepPin, ydirPin, en_y); // 建立左边步进电机对象

void setup()
{
  Serial.begin(115200);

  pinMode(en_x, OUTPUT);
  pinMode(xdirPin, OUTPUT);
  pinMode(xstepPin, OUTPUT);
  pinMode(en_y, OUTPUT);
  pinMode(ydirPin, OUTPUT);
  pinMode(ystepPin, OUTPUT);

  stepper1.setMaxSpeed(1000.0);    // 设置电机最大速度
  stepper1.setAcceleration(200.0); // 设置电机加速度
  stepper2.setMaxSpeed(1000.0);
  stepper2.setAcceleration(200.0);

  stepper1.setCurrentPosition(0); // 设置当前位置为电机零位
  stepper2.setCurrentPosition(0);
}

void loop()
{
  stepper1.move(2048);
  stepper2.move(-2048);
  stepper1.run();
  stepper2.run();
}