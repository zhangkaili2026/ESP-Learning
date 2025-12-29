#include <Arduino.h>

void setup() {
  // 【功能1】初始化UART0，波特率设置为实训要求的115200
  // ESP32-S3的Serial默认映射到UART0，无需额外引脚配置
  Serial.begin(115200);
  
  // 等待串口初始化完成，避免开机瞬间数据丢失
  while (!Serial) {
    delay(10);
  }

  // 串口打印初始化提示，方便确认程序启动
  Serial.println("ESP32-S3串口多字节收发程序已启动，等待接收数据...");
}

void loop() {
  // 【功能2】使用available()检测串口接收缓冲区是否有可读数据
  if (Serial.available() > 0) {
    // 延时10ms，确保多字节数据全部进入接收缓冲区（实训要求）
    delay(10);

    // 获取接收缓冲区中可读数据的长度
    int dataLen = Serial.available();
    // 定义数组存储接收的多字节数据（最大支持256字节，可按需调整）
    byte recvData[256];

    // 一次性读出缓冲区中所有数据
    Serial.readBytes(recvData, dataLen);

    // 向串口发送数据长度和数据内容（实训要求）
    Serial.print("接收数据长度：");
    Serial.println(dataLen);
    Serial.print("接收数据内容：");
    Serial.write(recvData, dataLen); // 以字节形式发送原始数据
    Serial.println(); // 换行分隔，便于查看
  }

  // 短暂延时，降低CPU占用（不影响串口检测）
  delay(1);
}