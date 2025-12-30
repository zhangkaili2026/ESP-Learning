#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// OLED屏幕参数（0.96寸I2C，分辨率128x64）
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // 无复位引脚设为-1
#define SCREEN_ADDRESS 0x3C // OLED I2C地址（0x3C或0x3D，依屏幕而定）

// D3灯引脚
#define LED_D3 3
// 闪烁间隔（0.5秒=500毫秒）
#define BLINK_INTERVAL 500

// 创建OLED对象
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// 闪烁次数计数器
int blinkCount = 0;
// 上一次灯状态切换的时间戳
unsigned long lastBlinkTime = 0;
// 灯的当前状态
bool ledState = LOW;

void setup() {
  // 初始化D3灯引脚为输出
  pinMode(LED_D3, OUTPUT);
  digitalWrite(LED_D3, ledState);

  // 初始化OLED屏幕
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while(1); // 初始化失败则死循环
  }

  // 清屏
  display.clearDisplay();
  
  // 第1行显示指定网址，1号字体
  display.setTextSize(1);        // 1号字体
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);      // 坐标(0,0)为第一行起始
  display.print("www.xmf393.com");

  // 第2行显示指定日期，1号字体
  display.setCursor(0, 10);     // 第二行，y轴偏移10像素
  display.print("2022-09-15");

  // 刷新OLED显示
  display.display();
}

void loop() {
  // 实现D3灯0.5秒闪烁
  if(millis() - lastBlinkTime >= BLINK_INTERVAL) {
    lastBlinkTime = millis();
    ledState = !ledState;
    digitalWrite(LED_D3, ledState);

    // 只有灯从灭变亮时，计数+1（一次完整闪烁计1次）
    if(ledState == HIGH) {
      blinkCount++;
      
      // 更新OLED第3行的闪烁次数，3号字体
      display.setTextSize(3);        // 3号字体
      display.setCursor(0, 30);     // 第三行，y轴偏移30像素
      display.print("LED: ");
      display.print(blinkCount);
      // 清掉数字后的残留像素
      display.fillRect(display.getCursorX(), 30, 40, 20, SSD1306_BLACK);
      display.display(); // 刷新显示
    }
  }
}