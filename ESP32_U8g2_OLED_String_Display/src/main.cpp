#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 初始化U8g2库（SSD1306 128x64 I2C，地址0x3C，无复位引脚）
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// D3灯引脚定义
#define LED_D3 3
// 闪烁间隔（0.5秒=500毫秒）
#define BLINK_INTERVAL 500
// 闪烁次数计数器
volatile int blinkCount = 0;
// 上一次灯状态切换的时间戳
unsigned long lastBlinkTime = 0;
// 灯的当前状态
bool ledState = LOW;

void setup() {
  // 初始化D3灯引脚为输出模式
  pinMode(LED_D3, OUTPUT);
  digitalWrite(LED_D3, ledState);

  // 初始化U8g2 OLED
  u8g2.begin();
  // 开启字体反走样（可选，让文字更清晰）
  u8g2.enableUTF8Print();
}

void loop() {
  // ===== D3灯0.5秒闪烁逻辑 =====
  if (millis() - lastBlinkTime >= BLINK_INTERVAL) {
    lastBlinkTime = millis();
    ledState = !ledState;
    digitalWrite(LED_D3, ledState);
    // 灯从灭变亮时，计数+1（一次完整闪烁计1次）
    if (ledState == HIGH) {
      blinkCount++;
    }
  }

  // ===== OLED显示逻辑 =====
  u8g2.firstPage();
  do {
    // 第1行：显示网址，字体u8g2_font_wqy14_t_gb2312
    u8g2.setFont(u8g2_font_wqy14_t_gb2312); // 设置14号文泉驿宋体
    u8g2.setCursor(0, 14); // 行高14，y坐标设为14（第一行基线）
    u8g2.print("www.xmf393.com");

    // 第2行：显示“中华人民共和国.广东”，字体u8g2_font_wqy12_t_gb2312
    u8g2.setFont(u8g2_font_wqy12_t_gb2312); // 设置12号文泉驿宋体
    u8g2.setCursor(0, 32); // 第二行基线，y坐标偏移18（14+18）
    u8g2.print("中华人民共和国.广东");

    // 第3行：动态显示闪烁次数，字体u8g2_font_wqy16_t_gb2312
    u8g2.setFont(u8g2_font_wqy16_t_gb2312); // 设置16号文泉驿宋体
    u8g2.setCursor(0, 52); // 第三行基线，y坐标再偏移20（32+20）
    u8g2.print("D3闪烁: ");
    u8g2.print(blinkCount);
    u8g2.print(" 次");

  } while (u8g2.nextPage()); // 循环刷新OLED页面
}