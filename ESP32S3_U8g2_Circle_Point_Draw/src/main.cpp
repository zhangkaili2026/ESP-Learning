#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 初始化U8g2库：SSD1306 128x64、I2C地址0x3C、无复位引脚
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// 定义U8g2绘制区域的宏（对应第二张图片的宏定义）
#define U8G2_DRAW_UPPER_LEFT    0x02    // 左上部分
#define U8G2_DRAW_LOWER_LEFT    0x04    // 左下部分
#define U8G2_DRAW_LOWER_RIGHT   0x08    // 右下部分
#define U8G2_DRAW_ALL           (0x01|0x02|0x04|0x08) // 全部区域

// 8个点的坐标数组
int points[8][2] = {
  {12,12}, {42,12}, {72,12}, {102,12}, // 上排4个点（空心圆的圆心）
  {12,44}, {42,44}, {72,44}, {102,44}  // 下排4个点（实心圆的圆心）
};

void setup() {
  // 初始化U8g2 OLED屏幕
  u8g2.begin();
  u8g2.clearBuffer(); // 清空缓冲区
}

void loop() {
  // ========== 步骤1：绘制8个点，每个间隔0.3秒 ==========
  for(int i=0; i<8; i++) {
    u8g2.drawPixel(points[i][0], points[i][1]); // 绘制单个像素点
    u8g2.sendBuffer(); // 刷新屏幕显示
    delay(300); // 间隔0.3秒
  }

  // ========== 步骤2：绘制4个空心圆（上排4个点为圆心，半径10），每个间隔0.5秒 ==========
  // 第1个空心圆：左上部分
  u8g2.drawCircle(points[0][0], points[0][1], 10, U8G2_DRAW_UPPER_LEFT);
  u8g2.sendBuffer();
  delay(500);
  // 第2个空心圆：左半部分（左上+左下）
  u8g2.drawCircle(points[1][0], points[1][1], 10, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_LOWER_LEFT);
  u8g2.sendBuffer();
  delay(500);
  // 第3个空心圆：左半+右下部分
  u8g2.drawCircle(points[2][0], points[2][1], 10, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_LOWER_LEFT | U8G2_DRAW_LOWER_RIGHT);
  u8g2.sendBuffer();
  delay(500);
  // 第4个空心圆：全部区域
  u8g2.drawCircle(points[3][0], points[3][1], 10, U8G2_DRAW_ALL);
  u8g2.sendBuffer();
  delay(500);

  // ========== 步骤3：绘制4个实心圆（下排4个点为圆心，半径12），每个间隔0.5秒 ==========
  // 第1个实心圆：左上部分
  u8g2.drawDisc(points[4][0], points[4][1], 12, U8G2_DRAW_UPPER_LEFT);
  u8g2.sendBuffer();
  delay(500);
  // 第2个实心圆：左半部分（左上+左下）
  u8g2.drawDisc(points[5][0], points[5][1], 12, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_LOWER_LEFT);
  u8g2.sendBuffer();
  delay(500);
  // 第3个实心圆：左半+右下部分
  u8g2.drawDisc(points[6][0], points[6][1], 12, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_LOWER_LEFT | U8G2_DRAW_LOWER_RIGHT);
  u8g2.sendBuffer();
  delay(500);
  // 第4个实心圆：全部区域
  u8g2.drawDisc(points[7][0], points[7][1], 12, U8G2_DRAW_ALL);
  u8g2.sendBuffer();
  delay(500);

  // ========== 步骤4：显示2秒后清屏1秒，循环绘制 ==========
  delay(2000); // 显示2秒
  u8g2.clearBuffer(); // 清空缓冲区
  u8g2.sendBuffer();  // 刷新清屏
  delay(1000); // 清屏后停留1秒
}