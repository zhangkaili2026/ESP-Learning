#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFi.h>

#define RELAY_PIN 4
// WiFi配置
const char *ssid = "今天也要好好学习呀";//WiFi名称
const char *password = "bsrz7937";//WiFi密码

// MQTT配置
const char *mqttBroker = "broker.emqx.io";
const int mqttPort = 1883;
const char *mqttUsername = "admin";
const char *mqttPassword = "admin";
String clientId;

// MQTT主题
const char *subscribeTopic = "zhangkaili/relay/control";
const char *publishTopic = "zhangkaili/sensor/temphum";

// 创建客户端对象
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// 函数声明
void onMqttMessageReceived(int messageSize);
void connectToMQTT();
void publishMessage(const char *topic, const char *payload); // 发布方法

void setup()
{
  Serial.begin(115200);

  pinMode(RELAY_PIN,OUTPUT);

  // 连接WiFi
  Serial.print("连接WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功！");

  // 生成ClientID
  clientId = "zhangkaili_ESP32_Client_" + String(millis());
  Serial.print("ClientID: ");
  Serial.println(clientId);

  // 连接MQTT
  connectToMQTT();
}

void loop()
{
  // 持续处理MQTT事件，确保实时接收消息
  mqttClient.poll();

  // 发送串口数据
  if (Serial.available())
  {
    String message = Serial.readStringUntil('\n');
    message.trim(); // 移除首尾空格
    Serial.print("发送消息: ");
    Serial.println(message);
    publishMessage(publishTopic, message.c_str());
  }
}

// MQTT连接函数
void connectToMQTT()
{
  Serial.print("连接MQTT服务器: ");
  Serial.println(mqttBroker);

  mqttClient.setId(clientId.c_str());
  mqttClient.setUsernamePassword(mqttUsername, mqttPassword);

  if (!mqttClient.connect(mqttBroker, mqttPort))
  {
    Serial.print("MQTT连接失败，错误码: ");
    Serial.println(mqttClient.connectError());
    return;
  }

  Serial.println("MQTT连接成功！");

  // 设置消息接收回调
  mqttClient.onMessage(onMqttMessageReceived);

  // 订阅主题
  if (mqttClient.subscribe(subscribeTopic))
  {
    Serial.println("订阅主题成功！");
  }
  else
  {
    Serial.println("订阅主题失败！");
  }
}

// 消息接收回调
void onMqttMessageReceived(int messageSize)
{
  Serial.print("\n收到消息 - 主题: ");
  Serial.print(mqttClient.messageTopic());
  Serial.print(" 内容: ");

  // 读取消息内容
  String message;
  while (mqttClient.available())
  {
    message += (char)mqttClient.read();
  }
  Serial.println(message);


if(message=="on")
{
  digitalWrite(RELAY_PIN,HIGH);
}
else if(message=="off")
{
  digitalWrite(RELAY_PIN,LOW);
}
}
// 发布方法
void publishMessage(const char *topic, const char *payload)
{
  mqttClient.beginMessage(topic);
  mqttClient.print(payload);
  mqttClient.endMessage();
  Serial.print("发布消息: ");
  Serial.print(topic);
  Serial.print(" -> ");
  Serial.println(payload);
}