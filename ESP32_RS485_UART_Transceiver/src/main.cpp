#include <Arduino.h>

// 定义串口与引脚
#define UART0_BAUD 115200    // UART0 波特率
#define UART2_BAUD 115200    // UART2（RS485）波特率
#define RS485_DE_RE 4        // RS485 模块的DE/RE使能引脚
#define LED_D3 3             // D3灯引脚

void setup() {
  // 初始化UART0（原生USB串口）
  Serial.begin(UART0_BAUD);
  while (!Serial) {
    delay(10); // 等待串口就绪
  }

  // 初始化UART2（RS485对应的串口），使用库中默认的Serial2
  // rx=16, tx=17 是ESP32-S3的UART2引脚映射
  Serial2.begin(UART2_BAUD, SERIAL_8N1, 16, 17);
  
  // 初始化RS485使能引脚（推挽输出，高电平为发送模式）
  pinMode(RS485_DE_RE, OUTPUT);
  digitalWrite(RS485_DE_RE, HIGH); // 全双工下持续置高，简化操作

  // 初始化D3灯引脚
  pinMode(LED_D3, OUTPUT);
  digitalWrite(LED_D3, LOW); // 初始灯灭

  Serial.println("RS485与UART互传程序启动");
  Serial2.println("RS485串口就绪");
}

void loop() {
  // 1. 读取UART0数据，转发到UART2（RS485）
  if (Serial.available() > 0) {
    uint8_t data = Serial.read(); // 读取UART0字节
    Serial2.write(data);          // 转发到RS485（UART2）
    Serial.printf("UART0→RS485: 0x%02X\n", data); // 调试打印
  }

  // 2. 读取UART2（RS485）数据，转发到UART0；若为0x55则翻转D3灯
  if (Serial2.available() > 0) {
    uint8_t data = Serial2.read(); // 读取RS485字节
    Serial.write(data);            // 转发到UART0
    Serial.printf("RS485→UART0: 0x%02X\n", data); // 调试打印

    // 判断是否为0x55，翻转D3灯
    if (data == 0x55) {
      digitalWrite(LED_D3, !digitalRead(LED_D3));
      Serial.println("收到0x55，D3灯状态翻转");
    }
  }
}