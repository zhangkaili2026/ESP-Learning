/**
 * ESP32-S3-N16R8 舵机控制程序
 * 作者：张凯莉
 * 日期：2025年
 * 
 * 硬件连接：
 * 舵机红线(电源+) → ESP32 5V或外部电源5V
 * 舵机棕线/黑线(地线) → ESP32 GND
 * 舵机橙线/黄线(信号线) → GPIO14
 * 
 * 重要提示：
 * 1. 如果控制多个舵机，请使用外部5V电源供电
 * 2. ESP32的3.3V输出可能不足以驱动舵机
 * 3. 确保GND共地（ESP32和电源的GND要连接）
 */
#include <Arduino.h>
// ========== 【必看】引脚设置 ==========
// 修改这里来改变舵机连接的引脚
#define SERVO_PIN 14  // 舵机信号线连接的GPIO引脚
// 舵机参数设置（根据你的舵机调整）
const int SERVO_MIN_PULSE = 500;    // 最小脉冲宽度（微秒）
const int SERVO_MAX_PULSE = 2500;   // 最大脉冲宽度（微秒）
const int SERVO_MIN_ANGLE = 0;      // 最小角度（度）
const int SERVO_MAX_ANGLE = 180;    // 最大角度（度）
// PWM设置（ESP32-S3专用）
#define SERVO_PWM_CHANNEL 0    // PWM通道，0-15，每个舵机用不同通道
#define SERVO_PWM_FREQ 50      // 标准舵机频率50Hz（周期20ms）
#define SERVO_PWM_RESOLUTION 16 // 分辨率16位（0-65535）
// ========== 舵机控制类 ==========
class ServoController {
private:
    int servoPin;
    int pwmChannel;
    int currentAngle;      // 当前角度
    int targetAngle;       // 目标角度
    int speed;             // 转动速度（度/秒）
    unsigned long lastUpdateTime;
    bool isMoving;         // 是否正在转动
    
    // 舵机脉冲参数（现在可以修改了）
    int servoMinPulse;
    int servoMaxPulse;
    
public:
    // 构造函数
    ServoController(int pin, int channel = SERVO_PWM_CHANNEL) {
        servoPin = pin;
        pwmChannel = channel;
        currentAngle = 90;   // 默认中间位置
        targetAngle = 90;
        speed = 90;          // 默认速度90度/秒
        isMoving = false;
        lastUpdateTime = 0;
        servoMinPulse = SERVO_MIN_PULSE;
        servoMaxPulse = SERVO_MAX_PULSE;
    }
    
    // 初始化函数
    void begin() {
        // 配置PWM
        ledcSetup(pwmChannel, SERVO_PWM_FREQ, SERVO_PWM_RESOLUTION);
        ledcAttachPin(servoPin, pwmChannel);
        
        // 设置初始位置（90度）
        writeAngle(currentAngle);
        
        Serial.println("舵机控制器初始化完成！");
        Serial.print("信号引脚: GPIO");
        Serial.println(servoPin);
        Serial.print("PWM通道: ");
        Serial.println(pwmChannel);
        Serial.print("频率: ");
        Serial.print(SERVO_PWM_FREQ);
        Serial.println("Hz");
        Serial.print("分辨率: ");
        Serial.print(SERVO_PWM_RESOLUTION);
        Serial.println("位");
        Serial.print("脉冲范围: ");
        Serial.print(servoMinPulse);
        Serial.print(" - ");
        Serial.print(servoMaxPulse);
        Serial.println(" 微秒");
    }
    
    // 计算角度对应的占空比
    int angleToDuty(int angle) {
        // 限制角度范围
        angle = constrain(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
        
        // 计算脉冲宽度（微秒）
        long pulseWidth = map(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, 
                              servoMinPulse, servoMaxPulse);
        
        // 将脉冲宽度转换为占空比
        // 占空比 = (脉冲宽度 / 周期) * 最大计数值
        long duty = (pulseWidth * (1 << SERVO_PWM_RESOLUTION)) / (1000000 / SERVO_PWM_FREQ);
        
        return (int)duty;
    }
    
    // 直接写入角度（立即转动）
    void writeAngle(int angle) {
        angle = constrain(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
        currentAngle = angle;
        targetAngle = angle;
        
        int duty = angleToDuty(angle);
        ledcWrite(pwmChannel, duty);
        
        Serial.print("舵机转到: ");
        Serial.print(angle);
        Serial.println("°");
    }
    
    // 平滑转动到指定角度
    void smoothMoveTo(int angle, int moveSpeed = -1) {
        if (moveSpeed != -1) {
            speed = moveSpeed;
        }
        
        angle = constrain(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
        targetAngle = angle;
        isMoving = true;
        lastUpdateTime = millis();
        
        Serial.print("平滑转动到: ");
        Serial.print(angle);
        Serial.print("°，速度: ");
        Serial.print(speed);
        Serial.println("°/秒");
    }
    
// 更新函数（需要在loop中调用以实现平滑转动）
    void update() {
        if (!isMoving || currentAngle == targetAngle) {
            return;
        }
        
        unsigned long currentTime = millis();
        unsigned long elapsedTime = currentTime - lastUpdateTime;
        
        if (elapsedTime >= 20) {  // 每20ms更新一次
            // 计算这20ms内应该转动的角度
            float angleStep = (speed * elapsedTime) / 1000.0;
            
            if (currentAngle < targetAngle) {
                currentAngle += angleStep;
                if (currentAngle > targetAngle) {
                    currentAngle = targetAngle;
                }
            } else {
                currentAngle -= angleStep;
                if (currentAngle < targetAngle) {
                    currentAngle = targetAngle;
                }
            }
            
            // 写入当前角度
            int duty = angleToDuty((int)currentAngle);
            ledcWrite(pwmChannel, duty);
            
            // 检查是否到达目标
            if (currentAngle == targetAngle) {
                isMoving = false;
                Serial.println("到达目标角度！");
            }
            
            lastUpdateTime = currentTime;
        }
    }
    
    // 设置转动速度
    void setSpeed(int newSpeed) {
        speed = constrain(newSpeed, 1, 180);
        Serial.print("速度设置为: ");
        Serial.print(speed);
        Serial.println("°/秒");
    }
    
    // 获取当前角度
    int getCurrentAngle() {
        return currentAngle;
    }
    
    // 获取目标角度
    int getTargetAngle() {
        return targetAngle;
    }
    
    // 检查是否正在转动
    bool isMovingNow() {
        return isMoving;
    }
    
    // 停止转动（保持当前位置）
    void stop() {
        isMoving = false;
        Serial.println("停止转动");
    }
    
    // 扫描模式（自动来回转动）
    void scanMode(int minAngle = 0, int maxAngle = 180, int scanSpeed = 60) {
        static bool forward = true;
        static int scanAngle = minAngle;
        
        if (forward) {
            scanAngle += 1;
            if (scanAngle >= maxAngle) {
                forward = false;
            }
        } else {
            scanAngle -= 1;
            if (scanAngle <= minAngle) {
                forward = true;
            }
        }
        
        writeAngle(scanAngle);
        delay(1000 / scanSpeed);  // 控制扫描速度
    }
    
    // 测试舵机全范围
    void testFullRange() {
        Serial.println("=== 测试舵机全范围 ===");
        
        Serial.println("转到0度...");
        writeAngle(0);
        delay(1000);
        
        Serial.println("转到90度...");
        writeAngle(90);
        delay(1000);
        
        Serial.println("转到180度...");
        writeAngle(180);
        delay(1000);
        
        Serial.println("转回90度...");
        writeAngle(90);
        
        Serial.println("测试完成！");
    }
    
    // 摇摆模式（像钟摆一样）
    void swingMode(int centerAngle = 90, int swingRange = 30, int swingSpeed = 45) {
        static int direction = 1;
        static int swingAngle = centerAngle;
        
        swingAngle += direction;
        
        if (swingAngle >= centerAngle + swingRange || swingAngle <= centerAngle - swingRange) {
            direction = -direction;
        }
        
        writeAngle(swingAngle);
        delay(1000 / swingSpeed);
    }
    
    // 设置自定义脉冲范围（兼容不同舵机）
    void setPulseRange(int minPulse, int maxPulse) {
        servoMinPulse = minPulse;
        servoMaxPulse = maxPulse;
        Serial.print("脉冲范围设置为: ");
        Serial.print(minPulse);
        Serial.print(" - ");
        Serial.print(maxPulse);
        Serial.println(" 微秒");
    }
};
// ========== 全局对象 ==========
ServoController servo(SERVO_PIN);
// ========== 串口菜单 ==========
void showMenu() {
    Serial.println("\n=================================");
    Serial.println("     ESP32-S3 舵机控制菜单");
    Serial.println("=================================");
    Serial.println("【基本控制】");
    Serial.println("  0 - 转到0度");
    Serial.println("  1 - 转到45度");
    Serial.println("  2 - 转到90度（中间）");
    Serial.println("  3 - 转到135度");
    Serial.println("  4 - 转到180度");
    
    Serial.println("\n【自定义角度】");
    Serial.println("  a [角度] - 直接转到指定角度");
    Serial.println("  示例: a 60  (转到60度)");
    
    Serial.println("\n【平滑转动】");
    Serial.println("  s [角度] - 平滑转动到指定角度");
    Serial.println("  示例: s 120  (平滑转到120度)");
    
    Serial.println("\n【速度控制】");
    Serial.println("  v [速度] - 设置转动速度(1-180度/秒)");
    Serial.println("  示例: v 60  (设置速度为60度/秒)");
    
    Serial.println("\n【特殊模式】");
    Serial.println("  t - 测试全范围");
    Serial.println("  c - 扫描模式（来回转动）");
    Serial.println("  w - 摇摆模式（钟摆）");
    Serial.println("  x - 停止转动");
    
    Serial.println("\n【状态查询】");
    Serial.println("  ? - 显示当前状态");
    Serial.println("  m - 显示此菜单");
    Serial.println("=================================\n");
}
// 显示当前状态
void showStatus() {
    Serial.println("\n=== 舵机状态 ===");
    Serial.print("当前角度: ");
    Serial.print(servo.getCurrentAngle());
    Serial.println("°");
    
    Serial.print("目标角度: ");
    Serial.print(servo.getTargetAngle());
    Serial.println("°");
    
    Serial.print("转动状态: ");
    Serial.println(servo.isMovingNow() ? "正在转动" : "静止");
    
    Serial.print("信号引脚: GPIO");
    Serial.println(SERVO_PIN);
    
    Serial.print("脉冲范围: ");
    Serial.print(SERVO_MIN_PULSE);
    Serial.print(" - ");
    Serial.print(SERVO_MAX_PULSE);
    Serial.println(" 微秒");
}
// ========== 主程序 ==========
void setup() {
    // 初始化串口
    Serial.begin(115200);
    delay(1000);  // 等待串口稳定
    
    Serial.println("\n************************************************");
    Serial.println("*          ESP32-S3-N16R8 舵机控制          *");
    Serial.println("*         Board: ESP32-S3-DevKitC-1          *");
    Serial.println("************************************************");
    
    // 初始化舵机
    servo.begin();
    
    // 显示欢迎信息
    Serial.println("\n欢迎使用舵机控制器！");
    Serial.println("重要提示：");
    Serial.println("1. 确保舵机电源充足（建议使用外部5V电源）");
    Serial.println("2. 确保ESP32和电源GND共地");
    Serial.println("3. 舵机信号线连接到 GPIO" + String(SERVO_PIN));
    
    // 显示菜单
    showMenu();
    
    // 显示初始状态
    showStatus();
}
void loop() {
    // 更新舵机状态（实现平滑转动）
    servo.update();
    
    // 检查串口输入
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();  // 去除首尾空格
        
        if (input.length() > 0) {
            char command = input.charAt(0);
            
            switch (command) {
                // 基本控制
                case '0':
                    Serial.println("> 转到0度");
                    servo.writeAngle(0);
                    break;
                    
                case '1':
                    Serial.println("> 转到45度");
                    servo.writeAngle(45);
                    break;
                    
                case '2':
                    Serial.println("> 转到90度");
                    servo.writeAngle(90);
                    break;
                    
                case '3':
                    Serial.println("> 转到135度");
                    servo.writeAngle(135);
                    break;
                    
                case '4':
                    Serial.println("> 转到180度");
                    servo.writeAngle(180);
                    break;
                    
                // 自定义角度
                case 'a':
                    if (input.length() > 2) {
                        int angle = input.substring(2).toInt();
                        Serial.print("> 直接转到 ");
                        Serial.print(angle);
                        Serial.println("度");
                        servo.writeAngle(angle);
                    }
                    break;
                    
// 平滑转动
                case 's':
                    if (input.length() > 2) {
                        int angle = input.substring(2).toInt();
                        Serial.print("> 平滑转动到 ");
                        Serial.print(angle);
                        Serial.println("度");
                        servo.smoothMoveTo(angle);
                    }
                    break;
                    
                // 速度设置
                case 'v':
                    if (input.length() > 2) {
                        int speed = input.substring(2).toInt();
                        servo.setSpeed(speed);
                    }
                    break;
                    
                // 特殊模式
                case 't':
                    Serial.println("> 测试舵机全范围");
                    servo.testFullRange();
                    break;
                    
                case 'c':
                    Serial.println("> 进入扫描模式（按任意键退出）");
                    while (!Serial.available()) {
                        servo.scanMode(0, 180, 30);
                    }
                    Serial.read();  // 清空输入缓冲区
                    Serial.println("> 退出扫描模式");
                    servo.writeAngle(90);
                    break;
                    
                case 'w':
                    Serial.println("> 进入摇摆模式（按任意键退出）");
                    while (!Serial.available()) {
                        servo.swingMode(90, 45, 60);
                    }
                    Serial.read();  // 清空输入缓冲区
                    Serial.println("> 退出摇摆模式");
                    servo.writeAngle(90);
                    break;
                    
                case 'x':
                    Serial.println("> 停止转动");
                    servo.stop();
                    break;
                    
                // 状态查询
                case '?':
                    showStatus();
                    break;
                    
                case 'm':
                    showMenu();
                    break;
                    
                default:
                    Serial.println("> 未知命令，输入 'm' 查看菜单");
            }
        }
    }
    
    delay(10);  // 防止CPU占用过高
}