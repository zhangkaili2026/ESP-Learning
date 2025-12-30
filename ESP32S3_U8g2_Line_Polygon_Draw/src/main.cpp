#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 初始化U8g2库：SSD1306 128x64、I2C地址0x3C、无复位引脚
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
  // 初始化U8g2 OLED屏幕
  u8g2.begin();
  u8g2.clearBuffer(); // 清空缓冲区
}

void loop() {
  // ========== 步骤2：绘制水平直线 + 垂直直线 ==========
  // 绘制水平直线：起点(0,32)，长度128
  u8g2.drawHLine(0, 32, 128);
  u8g2.sendBuffer();
  delay(500); // 0.5秒后执行下一步

  // 绘制垂直直线：起点(64,0)，长度64
  u8g2.drawVLine(64, 0, 64);
  u8g2.sendBuffer();
  delay(500);

  // ========== 步骤3：左上部分绘制5条等长水平线（间隔5像素，0.2秒/条） ==========
  for (int i = 0; i < 5; i++) {
    // 起点(2,25)，每条y坐标上移5像素，长度50
    u8g2.drawHLine(2, 25 - 5 * i, 50);
    u8g2.sendBuffer();
    delay(200);
  }
  delay(500); // 0.5秒后执行下一步

  // ========== 步骤4：右上部分绘制5条等长垂直线（间隔10像素，0.2秒/条） ==========
  for (int i = 0; i < 5; i++) {
    // 起点(75,0)，每条x坐标右移10像素，长度30
    u8g2.drawVLine(75 + 10 * i, 0, 30);
    u8g2.sendBuffer();
    delay(200);
  }
  delay(500);

  // ========== 步骤5：绘制矩形、直线、三角形 ==========
  // 绘制空心矩形：起点(2,38)，长度50，宽度25
  u8g2.drawFrame(2, 38, 50, 25);
  u8g2.sendBuffer();
  delay(500);

  // 绘制实心矩形：起点(7,43)，长度40，宽度15
  u8g2.drawBox(7, 43, 40, 15);
  u8g2.sendBuffer();
  delay(500);

  // 绘制垂直线：起点(70,38)，长度25
  u8g2.drawVLine(70, 38, 25);
  u8g2.sendBuffer();
  delay(500);

  // 绘制水平线：起点(70,63)，长度50
  u8g2.drawHLine(70, 63, 50);
  u8g2.sendBuffer();
  delay(500);

  // 绘制斜线：起点(70,38)，终点(120,63)
  u8g2.drawLine(70, 38, 120, 63);
  u8g2.sendBuffer();
  delay(500);

  // 绘制实心三角形：三个点(75,38)、(120,38)、(120,60)
  u8g2.drawTriangle(75, 38, 120, 38, 120, 60);
  u8g2.sendBuffer();
  delay(500);

  // ========== 步骤6：显示2秒后清屏1秒，循环绘制 ==========
  delay(2000); // 显示2秒
  u8g2.clearBuffer(); // 清空缓冲区
  u8g2.sendBuffer();  // 刷新清屏
  delay(1000); // 清屏后停留1秒
}
