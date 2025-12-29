/**
 * ESP32-S3-N16R8 蜂鸣器控制程序
 * 作者：张凯莉
 * 日期：2025年
 * 
 * 硬件连接：
 * 1. 有源蜂鸣器：GPIO18 -> 蜂鸣器+，蜂鸣器- -> GND
 * 2. 无源蜂鸣器：GPIO19 -> 蜂鸣器+，蜂鸣器- -> GND（串联100Ω电阻）
 * 
 * 使用方法：
 * 1. 上传程序到ESP32-S3
 * 2. 打开串口监视器（115200波特率）
 * 3. 输入命令测试
 */

#include <Arduino.h>

// ========== 【必看】引脚设置 ==========
// 修改这里来改变引脚
#define ACTIVE_BUZZER_PIN   18  // 有源蜂鸣器引脚
#define PASSIVE_BUZZER_PIN  19  // 无源蜂鸣器引脚
#define PWM_CHANNEL         0   // PWM通道，0-15都可以

// ========== 蜂鸣器控制类 ==========
class BuzzerController {
private:
    int activePin, passivePin, pwmChannel;
    
public:
    // 构造函数
    BuzzerController(int active, int passive, int channel) {
        activePin = active;
        passivePin = passive;
        pwmChannel = channel;
    }
    
    // 初始化函数
    void begin() {
        // 有源蜂鸣器设置
        pinMode(activePin, OUTPUT);
        digitalWrite(activePin, LOW);
        
        // 无源蜂鸣器PWM设置
        ledcSetup(pwmChannel, 2000, 8);  // 频率2000Hz，8位精度
        ledcAttachPin(passivePin, pwmChannel);
        ledcWriteTone(pwmChannel, 0);    // 初始关闭
        
        Serial.println("蜂鸣器初始化完成！");
        Serial.print("有源蜂鸣器引脚: GPIO");
        Serial.println(activePin);
        Serial.print("无源蜂鸣器引脚: GPIO");
        Serial.println(passivePin);
    }
    
    // ========== 有源蜂鸣器控制 ==========
    
    // 打开有源蜂鸣器
    void activeOn() {
        digitalWrite(activePin, HIGH);
    }
    
    // 关闭有源蜂鸣器
    void activeOff() {
        digitalWrite(activePin, LOW);
    }
    
    // 有源蜂鸣器叫一声
    void activeBeep(int duration_ms) {
        activeOn();
        delay(duration_ms);
        activeOff();
    }
    
    // 有源蜂鸣器叫多次
    void activeBeepTimes(int times, int onTime = 200, int offTime = 200) {
        for (int i = 0; i < times; i++) {
            activeBeep(onTime);
            if (i < times - 1) delay(offTime);
        }
    }
    
    // 播放SOS求救信号（有源）
    void activeSOS() {
        // 三短（S）
        for (int i = 0; i < 3; i++) {
            activeBeep(200);
            delay(200);
        }
        delay(400);
        // 三长（O）
        for (int i = 0; i < 3; i++) {
            activeBeep(600);
            delay(200);
        }
        delay(400);
        // 三短（S）
        for (int i = 0; i < 3; i++) {
            activeBeep(200);
            delay(200);
        }
    }
    
    // ========== 无源蜂鸣器控制 ==========
    
    // 播放一个频率的声音
    void passiveTone(int frequency, int duration_ms = 0) {
        if (frequency == 0) {
            passiveStop();
            return;
        }
        ledcWriteTone(pwmChannel, frequency);
        if (duration_ms > 0) {
            delay(duration_ms);
            passiveStop();
        }
    }
    
    // 停止无源蜂鸣器
    void passiveStop() {
        ledcWriteTone(pwmChannel, 0);
    }
    
    // 播放音阶（Do Re Mi Fa Sol La Si Do）
    void passiveScale() {
        int notes[] = {262, 294, 330, 349, 392, 440, 494, 523};
        for (int i = 0; i < 8; i++) {
            passiveTone(notes[i], 300);
            delay(100);
        }
        passiveStop();
    }
    
    // 播放警报声（无源）
    void passiveAlarm(int cycles = 3) {
        for (int i = 0; i < cycles; i++) {
            passiveTone(800, 200);
            passiveTone(1600, 200);
        }
        passiveStop();
    }
    
    // 频率扫描效果
    void passiveSweep(int startFreq = 200, int endFreq = 2000, int step = 50, int delayTime = 20) {
        if (startFreq < endFreq) {
            for (int freq = startFreq; freq <= endFreq; freq += step) {
                passiveTone(freq, 0);
                delay(delayTime);
            }
        } else {
            for (int freq = startFreq; freq >= endFreq; freq -= step) {
                passiveTone(freq, 0);
                delay(delayTime);
            }
        }
        passiveStop();
    }
    
// ========== 综合控制 ==========
    
    // 停止所有蜂鸣器
    void stopAll() {
        activeOff();
        passiveStop();
    }
    
    // 播放启动提示音
    void playStartup() {
        activeBeep(100);
        delay(150);
        passiveTone(1000, 200);
        delay(100);
        passiveTone(1500, 300);
    }
    
    // 播放错误提示音
    void playError() {
        for (int i = 0; i < 3; i++) {
            passiveTone(300, 200);
            delay(150);
        }
    }
    
    // 播放成功提示音
    void playSuccess() {
        passiveTone(523, 150);  // Do
        delay(50);
        passiveTone(659, 150);  // Mi
        delay(50);
        passiveTone(784, 300);  // Sol
    }
    
    // 测试所有功能
    void testAll() {
        Serial.println("=== 开始全部测试 ===");
        
        Serial.println("1. 测试有源蜂鸣器...");
        activeBeepTimes(3, 100, 100);
        delay(500);
        
        Serial.println("2. 测试无源蜂鸣器音阶...");
        passiveScale();
        delay(500);
        
        Serial.println("3. 测试警报声...");
        passiveAlarm(2);
        delay(500);
        
        Serial.println("4. 测试频率扫描...");
        passiveSweep();
        delay(500);
        
        Serial.println("5. 播放提示音...");
        playStartup();
        delay(300);
        playError();
        delay(300);
        playSuccess();
        
        Serial.println("=== 测试完成 ===");
    }
};
// ========== 全局对象 ==========
BuzzerController buzzer(ACTIVE_BUZZER_PIN, PASSIVE_BUZZER_PIN, PWM_CHANNEL);
// ========== 串口菜单 ==========
void showMenu() {
    Serial.println("\n=================================");
    Serial.println("     ESP32-S3 蜂鸣器控制菜单");
    Serial.println("=================================");
    Serial.println("【有源蜂鸣器控制】");
    Serial.println("  1 - 短鸣一声 (200ms)");
    Serial.println("  2 - 长鸣一声 (1000ms)");
    Serial.println("  3 - 鸣三声");
    Serial.println("  4 - SOS求救信号");
    
    Serial.println("\n【无源蜂鸣器控制】");
    Serial.println("  5 - 播放音阶");
    Serial.println("  6 - 播放警报声");
    Serial.println("  7 - 频率扫描 (200-2000Hz)");
    Serial.println("  8 - 播放 1000Hz 声音");
    Serial.println("  9 - 播放 2000Hz 声音");
    
    Serial.println("\n【综合功能】");
    Serial.println("  s - 播放启动音");
    Serial.println("  e - 播放错误音");
    Serial.println("  u - 播放成功音");
    Serial.println("  t - 全部测试");
    Serial.println("  x - 停止所有");
    Serial.println("  m - 显示此菜单");
    Serial.println("=================================\n");
}
// ========== 主程序 ==========
void setup() {
    // 初始化串口
    Serial.begin(115200);
    delay(1000);  // 重要！等待串口稳定
    
    Serial.println("\n************************************************");
    Serial.println("*          ESP32-S3-N16R8 蜂鸣器控制          *");
    Serial.println("*         Board: ESP32-S3-DevKitC-1          *");
    Serial.println("************************************************");
    
    // 初始化蜂鸣器
    buzzer.begin();
    
    // 播放启动音
    buzzer.playStartup();
    
    // 显示菜单
    showMenu();
}
void loop() {
    // 检查串口输入
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        
        switch (cmd) {
            // 有源蜂鸣器控制
            case '1':
                Serial.println("> 有源蜂鸣器短鸣");
                buzzer.activeBeep(200);
                break;
                
            case '2':
                Serial.println("> 有源蜂鸣器长鸣");
                buzzer.activeBeep(1000);
                break;
                
            case '3':
                Serial.println("> 有源蜂鸣器鸣三声");
                buzzer.activeBeepTimes(3);
                break;
                
            case '4':
                Serial.println("> 有源蜂鸣器SOS信号");
                buzzer.activeSOS();
                break;
                
            // 无源蜂鸣器控制
            case '5':
                Serial.println("> 无源蜂鸣器音阶");
                buzzer.passiveScale();
                break;
                
            case '6':
                Serial.println("> 无源蜂鸣器警报声");
                buzzer.passiveAlarm(3);
                break;
                
            case '7':
                Serial.println("> 无源蜂鸣器频率扫描");
                buzzer.passiveSweep();
                break;
                
            case '8':
                Serial.println("> 无源蜂鸣器 1000Hz");
                buzzer.passiveTone(1000, 500);
                break;
                
            case '9':
                Serial.println("> 无源蜂鸣器 2000Hz");
                buzzer.passiveTone(2000, 500);
                break;
                
            // 综合功能
            case 's':
                Serial.println("> 播放启动音");
                buzzer.playStartup();
                break;
                
            case 'e':
                Serial.println("> 播放错误音");
                buzzer.playError();
                break;
                
            case 'u':
                Serial.println("> 播放成功音");
                buzzer.playSuccess();
                break;
                
            case 't':
                Serial.println("> 开始全部测试");
                buzzer.testAll();
                break;
                
            case 'x':
                Serial.println("> 停止所有蜂鸣器");
                buzzer.stopAll();
                break;
                
            case 'm':
                showMenu();
                break;
                
            case '\n':  // 忽略换行
            case '\r':  // 忽略回车
                break;
                
            default:
                Serial.println("> 未知命令，输入 'm' 查看菜单");
        }
    }
    
    delay(10);  // 防止CPU占用过高
}