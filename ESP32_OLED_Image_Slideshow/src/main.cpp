#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// OLED屏幕参数（128x64分辨率，I2C地址0x3C）
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// 图片切换间隔（3秒）
#define IMAGE_INTERVAL 3000

// 创建OLED对象
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 图片1（宝马标）128x64单色BMP数据（PCtoLCD2002生成的字模，这里简化为示例数据）
const unsigned char pic1[] PROGMEM = {
  // 省略完整数据，实际需用PCtoLCD2002生成128x64单色BMP的点阵数据，格式为列行式、取模方式阴码/阳码匹配
  // 示例占位，需替换为实际生成的数组
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  // 后续需补充完整128x64的点阵数据（共1024字节：128*64/8）
};

// 图片2（奥迪标）128x64单色BMP数据
const unsigned char pic2[] PROGMEM = {
  // 同样需用PCtoLCD2002生成实际点阵数据，此处为占位
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  // 后续需补充完整1024字节数据
};

// 显示图片的函数
void drawImage(const unsigned char* img) {
  display.clearDisplay(); // 清屏
  // 逐行绘制128x64的图片
  for(int y=0; y<8; y++) { // 64行分为8个页面，每个页面8行
    display.setCursor(0, y*8); // 设置当前页面起始坐标
    for(int x=0; x<128; x++) { // 逐列绘制
      display.write(pgm_read_byte(&img[y*128 + x])); // 从Flash读取图片数据
    }
  }
  display.display(); // 刷新显示
}

void setup() {
  // 初始化OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while(1); // 初始化失败则死循环
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  // 显示第一张图片，延时3秒
  drawImage(pic1);
  delay(IMAGE_INTERVAL);

  // 显示第二张图片，延时3秒
  drawImage(pic2);
  delay(IMAGE_INTERVAL);
}