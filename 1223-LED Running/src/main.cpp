#include <Arduino.h>
//定义LED引脚常量
const int LED_PIN_1 = 3;//D3
const int LED_PIN_2 = 4;
const int LED_PIN_3 = 5;
const int LED_PIN_4 = 6;
//定义延迟时间（毫秒）
const int DELAY_TIME = 500;//0.5秒等于500毫秒，1秒等于1000毫秒
//定义LED引脚数组，便于循环控制
int ledPins[] = {LED_PIN_1,LED_PIN_2,LED_PIN_3,LED_PIN_4};
int ledCount = 4;//LED数量

void setup() {
  //初始化串口通信，用于调试
  Serial.begin(115200);//初始化串口通信，波特率115200
  Serial.println("跑马灯程序开始初始化...");
  pinMode(LED_PIN_1,OUTPUT);
  pinMode(LED_PIN_2,OUTPUT);
  pinMode(LED_PIN_3,OUTPUT);
  pinMode(LED_PIN_4,OUTPUT);
  Serial.println("初始化完成，开始跑马灯效果");
}
void loop(){
  Serial.println("逐个点亮LED...");
  
  digitalWrite(LED_PIN_1,HIGH);//D3灯亮
  Serial.println("D3灯亮");
  delay(DELAY_TIME);//延时0.5秒

  digitalWrite(LED_PIN_2,HIGH);
  Serial.println("D4灯亮");
  delay(DELAY_TIME);

  digitalWrite(LED_PIN_3,HIGH);
  Serial.println("D5灯亮");
  delay(DELAY_TIME);

  digitalWrite(LED_PIN_4,HIGH);
  Serial.println("D6灯亮");
  delay(DELAY_TIME);

  //第二步，逐个熄灭LED
  Serial.println("逐个熄灭LED...");

  digitalWrite(LED_PIN_1,LOW);
  Serial.println("D3灯灭");
  delay(DELAY_TIME);

  digitalWrite(LED_PIN_2,LOW);
  Serial.println("D4灯灭");
  delay(DELAY_TIME);

  digitalWrite(LED_PIN_3,LOW);
  Serial.println("D5灯灭");
  delay(DELAY_TIME);

  digitalWrite(LED_PIN_4,LOW);
  Serial.println("D6灯灭");
  delay(DELAY_TIME);

  Serial.println("===循环结束===\n");
} 