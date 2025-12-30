#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 初始化U8g2库：SSD1306 128x64、I2C地址0x3C、无复位引脚
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// 进度条参数定义
#define BAR_NUM 20          // 进度条的实心矩形数量
#define BAR_W 3             // 单个矩形宽度
#define BAR_H 12            // 单个矩形高度
#define BAR_GAP 1           // 矩形之间的间隔
#define BAR_START_X 6       // 进度条起始X坐标
#define BAR_START_Y 40      // 进度条起始Y坐标（屏幕下半部分）
#define PERCENT_X 110       // 百分比显示X坐标（进度条最右侧）
#define PERCENT_Y 48        // 百分比显示Y坐标
#define SYMBOL_X 6          // 符号显示X坐标（屏幕左上角）
#define SYMBOL_Y 10         // 符号显示Y坐标

void setup() {
  // 初始化U8g2 OLED屏幕
  u8g2.begin();
  u8g2.clearBuffer(); // 清空缓冲区
  u8g2.setFont(u8g2_font_6x10_tf); // 设置字体（适配数字和中文显示）
}

void loop() {
  int progress = 0; // 进度百分比（0-100）
  u8g2.clearBuffer(); // 每次循环先清屏

  // ========== 绘制进度条主逻辑 ==========
  for (int i = 0; i < BAR_NUM; i++) {
    // 计算单个实心矩形的X坐标：起始X + (宽度+间隔)*序号
    int bar_x = BAR_START_X + (BAR_W + BAR_GAP) * i;
    // 绘制实心矩形（进度条的一个分段）
    u8g2.drawBox(bar_x, BAR_START_Y, BAR_W, BAR_H);
    u8g2.sendBuffer(); // 刷新屏幕显示

    // 计算当前进度百分比（每个矩形占5%）
    progress = (i + 1) * 5;
    // 在进度条右侧显示百分比
    u8g2.setCursor(PERCENT_X, PERCENT_Y);
    u8g2.print(progress);
    u8g2.print("%");
    u8g2.sendBuffer();

    // ========== 进度触发符号显示 ==========
    if (progress == 30) {
      // 30%时显示#，ASCII码0x23（对应第二张图片）
      u8g2.drawGlyph(SYMBOL_X, SYMBOL_Y, 0x23);
      u8g2.sendBuffer();
    }
    if (progress == 60) {
      // 60%时显示$，ASCII码0x24（对应第二张图片）
      u8g2.drawGlyph(SYMBOL_X + 10, SYMBOL_Y, 0x24);
      u8g2.sendBuffer();
    }

    delay(200); // 每隔0.2秒绘制一个矩形
  }

  // ========== 进度100%时的操作 ==========
  // 清掉原有内容，显示“完成！”
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy14_t_gb2312); // 切换为中文字体
  u8g2.setCursor(40, 32); // 屏幕中央坐标
  u8g2.print("完成！");
  // 在屏幕四周绘制矩形框（边框坐标：0,0 到 127,63）
  u8g2.drawFrame(0, 0, 127, 63);
  u8g2.sendBuffer();

  // ========== 显示2秒后清屏1秒，循环执行 ==========
  delay(2000); // 显示2秒
  u8g2.clearBuffer();
  u8g2.sendBuffer();
  delay(1000); // 清屏后停留1秒
}