#include <Arduino.h>

void setup() {
  // 【功能1】初始化UART0，波特率设置为实训要求的115200
  // ESP32-S3的Serial默认映射到UART0，无需额外引脚配置
  Serial.begin(115200);
  
  // 等待串口初始化完成，避免开机瞬间数据丢失
  while (!Serial) {
    delay(10);
  }

  // 串口打印初始化提示（可选，方便确认程序启动）
  Serial.println("ESP32-S3串口单字节收发程序已启动，等待接收数据...");
}

void loop() {
  // 【功能2】使用available()检测串口接收缓冲区是否有可读数据
  if (Serial.available() > 0) {
    // 使用read()读取一个字节的串口数据
    byte receivedData = Serial.read();
    
    // 将接收的字节数据加1
    byte sendData = receivedData + 1;
    
    // 使用write()将加1后的数据通过串口0发送回去
    Serial.write(sendData);

    // （可选）打印收发过程，方便调试查看
    Serial.printf("接收字节：0x%02X，发送字节（加1）：0x%02X\n", receivedData, sendData);
  }

  // 短暂延时，降低CPU占用（不影响串口检测）
  delay(1);
}
