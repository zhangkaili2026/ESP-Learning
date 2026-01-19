#include <Arduino.h>

#define pin 6

void setup()
{
  pinMode(pin,OUTPUT);
  digitalWrite(pin,HIGH);
}

void loop()
{
  digitalWrite(pin,LOW);
  delay (3000);
  digitalWrite(pin,HIGH);
  delay (3000);
}