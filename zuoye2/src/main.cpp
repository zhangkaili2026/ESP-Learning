#include <Arduino.h>
#include <AccelStepper.h>

// 风扇控制引脚
#define FAN_PIN 18

// 步进电机引脚
#define EN_X 6      // 左电机使能
#define X_DIR_PIN 4  // 左电机方向
#define X_STEP_PIN 5 // 左电机脉冲
#define EN_Y 45      // 右电机使能
#define Y_DIR_PIN 47 // 右电机方向
#define Y_STEP_PIN 48// 右电机脉冲

// 创建步进电机对象（脉冲+方向模式）
AccelStepper stepper1(1, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepper2(1, Y_STEP_PIN, Y_DIR_PIN);

// 系统状态变量
enum SystemState {
  STATE_AUTO,    // 自动模式：电机转10秒，停10秒，风扇同步
  STATE_OFF      // 关闭模式：风扇关闭，电机停转
};

SystemState currentState = STATE_AUTO; // 初始为自动模式
bool systemEnabled = true;             // 系统总开关

// 时间控制变量
unsigned long previousMillis = 0;
unsigned long fanMotorStartTime = 0;
bool fanMotorRunning = false;          // 电机和风扇运行状态
unsigned long motorRunDuration = 10000;  // 电机运行10秒
unsigned long motorStopDuration = 10000; // 电机停止10秒

// 串口输入缓冲
String inputString = "";
bool stringComplete = false;

// 函数声明
void autoMode();
void offMode();
void checkSerialInput();

void setup() {
  // 初始化风扇引脚
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);

  // 初始化电机使能引脚
  pinMode(EN_X, OUTPUT);
  pinMode(EN_Y, OUTPUT);
  
  // 使能电机（低电平有效）
  digitalWrite(EN_X, LOW);
  digitalWrite(EN_Y, LOW);

  // 串口初始化
  Serial.begin(115200);
  Serial.println("====== 系统启动 ======");
  Serial.println("系统初始状态：自动模式");
  Serial.println("电机：运行10秒，停止10秒，风扇同步");
  Serial.println("输入 'OFF' 关闭系统");
  Serial.println("输入 'AUTO' 恢复自动模式");
  Serial.println("===========================");

  // 电机参数设置
  stepper1.setMaxSpeed(800.0);
  stepper1.setAcceleration(400.0);
  stepper2.setMaxSpeed(800.0);
  stepper2.setAcceleration(400.0);
  
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);
  
  // 初始移动
  stepper1.move(2048);
  stepper2.move(-2048);
  
  // 记录开始时间
  fanMotorStartTime = millis();
  fanMotorRunning = true;
}

void loop() {
  // 检查串口输入
  checkSerialInput();
  
  // 根据系统状态执行相应操作
  switch (currentState) {
    case STATE_AUTO:
      autoMode();
      break;
    case STATE_OFF:
      offMode();
      break;
  }
  
  // 只有在自动模式且系统启用时才运行电机
  if (currentState == STATE_AUTO && systemEnabled) {
    stepper1.run();
    stepper2.run();
  }
}

// 自动模式：电机运行10秒，停止10秒，风扇同步
void autoMode() {
  unsigned long currentMillis = millis();
  
  if (fanMotorRunning) {
    // 电机和风扇正在运行
    if (currentMillis - fanMotorStartTime >= motorRunDuration) {
      // 运行时间到，停止
      fanMotorRunning = false;
      fanMotorStartTime = currentMillis;
      
      // 关闭风扇
      digitalWrite(FAN_PIN, LOW);
      
      // 停止电机（设置当前位置为目标位置）
      stepper1.stop();
      stepper2.stop();
      stepper1.setCurrentPosition(stepper1.currentPosition());
      stepper2.setCurrentPosition(stepper2.currentPosition());
      
      Serial.println("状态：电机和风扇停止");
      Serial.print("时间：");
      Serial.println(millis() / 1000);
    } else {
      // 保持风扇开启
      digitalWrite(FAN_PIN, HIGH);
    }
  } else {
    // 电机和风扇停止中
    if (currentMillis - fanMotorStartTime >= motorStopDuration) {
      // 停止时间到，重新启动
      fanMotorRunning = true;
      fanMotorStartTime = currentMillis;
      
      // 开启风扇
      digitalWrite(FAN_PIN, HIGH);
      
      // 设置电机反向运动
      long currentPos1 = stepper1.currentPosition();
      long currentPos2 = stepper2.currentPosition();
      
      if (currentPos1 >= 2048) {
        stepper1.move(0);
        stepper2.move(0);
      } else {
        stepper1.move(2048);
        stepper2.move(-2048);
      }
      
      Serial.println("状态：电机和风扇启动");
      Serial.print("时间：");
      Serial.println(millis() / 1000);
      Serial.print("电机1目标位置：");
      Serial.print(stepper1.targetPosition());
      Serial.print(" | 电机2目标位置：");
      Serial.println(stepper2.targetPosition());
    }
  }
}

// 关闭模式：风扇关闭，电机停转
void offMode() {
  // 确保风扇关闭
  digitalWrite(FAN_PIN, LOW);
  
  // 停止电机
  stepper1.stop();
  stepper2.stop();
}

// 检查和处理串口输入
void checkSerialInput() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      if (inputString.length() > 0) {
        stringComplete = true;
      }
    } else {
      inputString += inChar;
    }
  }
  
  if (stringComplete) {
    // 转换为大写便于比较
    inputString.toUpperCase();
    inputString.trim();
    
    Serial.print("收到命令：");
    Serial.println(inputString);
    
    if (inputString == "OFF") {
      currentState = STATE_OFF;
      systemEnabled = false;
      digitalWrite(FAN_PIN, LOW);
      stepper1.stop();
      stepper2.stop();
      Serial.println("系统已关闭");
      Serial.println("风扇：关闭");
      Serial.println("电机：停止");
    } 
    else if (inputString == "AUTO") {
      currentState = STATE_AUTO;
      systemEnabled = true;
      fanMotorStartTime = millis();
      fanMotorRunning = true;
      
      // 重新设置电机运动
      digitalWrite(EN_X, LOW);  // 使能电机
      digitalWrite(EN_Y, LOW);
      
      long currentPos1 = stepper1.currentPosition();
      if (currentPos1 >= 2048) {
        stepper1.move(0);
        stepper2.move(0);
      } else {
        stepper1.move(2048);
        stepper2.move(-2048);
      }
      
      Serial.println("系统恢复自动模式");
      Serial.println("电机：运行10秒，停止10秒");
      Serial.println("风扇：与电机同步");
    }
    else {
      Serial.println("未知命令");
      Serial.println("可用命令：");
      Serial.println("  OFF  - 关闭系统");
      Serial.println("  AUTO - 自动模式");
    }
    
    // 清空输入缓冲
    inputString = "";
    stringComplete = false;
  }
}