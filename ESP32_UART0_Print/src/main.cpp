#include <Arduino.h>

// 定义全局变量num，用于累计运行次数
int num = 0;

void setup() {
  // 初始化UART0串口，波特率设置为实训要求的115200
  // ESP32-S3的Serial默认映射到UART0，对应硬件串口的TX/RX
  Serial.begin(115200);
  
  // 等待串口初始化完成（避免开机瞬间打印丢失）
  while (!Serial) {
    delay(10);
  }
}

void loop() {
  // 【功能2】使用print()打印指定字符串，无换行
  Serial.print("Uart0:");
  Serial.print("Hello World!");
  // 使用println()实现回车换行
  Serial.println();

  // 【功能3】num变量加1，累计运行次数
  num++;
  // 使用printf()格式化打印运行次数，回车换行
  Serial.printf("运行次数：%d次\n", num);

  // 【功能4】以二进制形式打印num变量
  Serial.print("num的二进制为：");
  Serial.println(num, BIN); // BIN表示二进制格式输出
  // 以十六进制形式打印num变量
  Serial.print("num的十六进制为：");
  Serial.println(num, HEX); // HEX表示十六进制格式输出

  // 空一行分隔每次输出，便于查看
  Serial.println();

  // 【功能3】每隔2秒执行一次循环（2000毫秒）
  delay(2000);
}
