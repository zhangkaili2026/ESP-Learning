#include <Arduino.h>

#define pin 6

void setup()
{
  pinMode(pin,OUTPUT);
}

void loop()
{
  digitalWrite(pin,HIGH);
}