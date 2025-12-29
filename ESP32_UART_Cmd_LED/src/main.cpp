#include <Arduino.h>

// 定义LED引脚（适配ESP32-S3，可根据实际接线修改）
const int D4_LED = 27;
const int D6_LED = 33;

void setup() {
  // 【功能1】初始化UART0，波特率设置为实训要求的115200
  Serial.begin(115200);
  // 等待串口初始化完成，避免数据丢失
  while (!Serial) {
    delay(10);
  }

  // 初始化LED引脚为输出模式
  pinMode(D4_LED, OUTPUT);
  pinMode(D6_LED, OUTPUT);
  // 初始关闭所有LED
  digitalWrite(D4_LED, LOW);
  digitalWrite(D6_LED, LOW);

  // 串口打印初始化提示
  Serial.println("ESP32-S3串口单字节命令控灯程序已启动，等待接收命令...");
  Serial.println("命令列表：0xA1-点亮D4 | 0xA2-点亮D6 | 0xB1-熄灭D4 | 0xB2-熄灭D6");
}

void loop() {
  // 检测串口接收缓冲区是否有可读数据
  if (Serial.available() > 0) {
    // 读取一个字节的命令数据
    byte cmd = Serial.read();

    // 【功能2】解析串口单字节命令
    switch (cmd) {
      case 0xA1: // 点亮D4灯命令
        digitalWrite(D4_LED, HIGH);
        Serial.println("D4 is opened!");
        break;
      case 0xA2: // 点亮D6灯命令
        digitalWrite(D6_LED, HIGH);
        Serial.println("D6 is opened!");
        break;
      case 0xB1: // 熄灭D4灯命令
        digitalWrite(D4_LED, LOW);
        Serial.println("D4 is closed!");
        break;
      case 0xB2: // 熄灭D6灯命令
        digitalWrite(D6_LED, LOW);
        Serial.println("D6 is closed!");
        break;
      default: // 非指定命令
        Serial.println("ERROR!!!");
        break;
    }
  }

  // 短暂延时，降低CPU占用
  delay(1);
}