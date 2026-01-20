#include <Arduino.h>
#include <AccelStepper.h>

// 引脚定义
#define en_x 6      // 电机使能引脚
#define xdirPin 4   // 电机方向引脚
#define xstepPin 5  // 电机脉冲引脚
#define fanPin 18    // 风扇控制引脚 (新增)

// 建立步进电机对象
AccelStepper stepper(1, xstepPin, xdirPin);

// 全局变量
unsigned long moveStartTime = 0; // 记录电机开始运动的时间
bool fanAutoMode = false;        // 是否处于自动开启风扇状态

void setup() {
  Serial.begin(115200);
  
  // 引脚初始化
  pinMode(en_x, OUTPUT);
  digitalWrite(en_x, LOW); 
  
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, LOW); // 默认关闭风扇

  // 电机初始化
  stepper.setMaxSpeed(2000.0);     
  stepper.setAcceleration(500.0);  
  stepper.setCurrentPosition(0);   

  Serial.println(F("--- 电机与风扇综合控制系统 ---"));
  Serial.println(F("指令列表:"));
  Serial.println(F("FAN 1/0        - 手动开关风扇"));
  Serial.println(F("RUN [方向] [转速] [角度] - 正/反转运动 (方向: 1正, -1反)"));
  Serial.println(F("OFF            - 关闭风扇并停止电机"));
  Serial.println(F("STATUS         - 查看当前状态"));
}

void loop() {
  // 1. 串口指令解析
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); 
    input.trim(); 
    if (input.length() == 0) return; 
    
    input.toUpperCase(); 

    int spaceIndex = input.indexOf(' ');
    String command = (spaceIndex != -1) ? input.substring(0, spaceIndex) : input;
    String valueStr = (spaceIndex != -1) ? input.substring(spaceIndex + 1) : "";

    // --- 风扇控制 ---
    if (command == "FAN") {
      int state = valueStr.toInt();
      digitalWrite(fanPin, state == 1 ? HIGH : LOW);
      Serial.print(F(">> 风扇已: "));
      Serial.println(state == 1 ? F("开启") : F("关闭"));
    } 
    
    // --- 综合运动控制: RUN [方向] [转速] [角度] ---
    else if (command == "RUN") {
      // 解析参数 (格式: RUN 1 1000 360)
      int p1 = valueStr.indexOf(' ');
      int p2 = valueStr.lastIndexOf(' ');
      
      if (p1 != -1 && p2 != -1 && p1 != p2) {
        int dir = valueStr.substring(0, p1).toInt();      // 方向: 1 或 -1
        float speed = valueStr.substring(p1 + 1, p2).toFloat(); // 转速
        float angle = valueStr.substring(p2 + 1).toFloat();   // 角度
        
        // 假设电机 200 步转一圈 (根据实际驱动器细分修改)
        long steps = (angle / 360.0) * 200.0 * 16.0; // 此处假设16细分，总步数请按需调整
        
        stepper.setMaxSpeed(speed);
        stepper.move(dir * steps);
        
        moveStartTime = millis(); // 记录开始运动的时间
        fanAutoMode = true;       // 激活自动风扇监控
        
        Serial.print(F(">> 启动运动: 方向=")); Serial.print(dir);
        Serial.print(F(" 速度=")); Serial.print(speed);
        Serial.print(F(" 步数=")); Serial.println(dir * steps);
      } else {
        Serial.println(F(">> 参数错误! 格式: RUN [方向] [转速] [角度]"));
      }
    }

    // --- 全停指令 ---
    else if (command == "OFF") {
      stepper.stop();
      digitalWrite(fanPin, LOW);
      fanAutoMode = false;
      Serial.println(F(">> 系统强制关闭 (风扇关/电机停)"));
    }

    else if (command == "STATUS") {
      Serial.print(F(">> 位置: ")); Serial.print(stepper.currentPosition());
      Serial.print(F(" | 风扇状态: ")); Serial.println(digitalRead(fanPin));
    }
  }

  // 2. 核心逻辑: 连续运动 10 秒自动开风扇
  if (stepper.distanceToGo() != 0) {
    // 电机正在运行中
    if (fanAutoMode && digitalRead(fanPin) == LOW) {
      if (millis() - moveStartTime >= 10000) { // 达到10000毫秒 (10秒)
        digitalWrite(fanPin, HIGH);
        Serial.println(F(">> [自动] 电机运行已达10秒，风扇开启"));
      }
    }
  } else {
    // 电机停止运动时重置计时
    moveStartTime = 0;
  }

  // 3. 电机驱动核心
  stepper.run();
}