/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This example shows how value can be pushed from Arduino to
  the Blynk App.

  WARNING :
  For this example you'll need Adafruit DHT sensor libraries:
    https://github.com/adafruit/Adafruit_Sensor
    https://github.com/adafruit/DHT-sensor-library

  App project setup:
    Value Display widget attached to V5
    Value Display widget attached to V6
 *************************************************************/

/* Comment this out to disable prints and save space */
//**********************************************************
//V6-датчик температуры(t)
//V5-датчик влажности(h)
//V4-датчик увлажнённости земли(val)
//V3-серво-дверь
//V1-серво-вода
//V2-режимы (auto/hand)
//V7-минимальная температура (slider)
//V8-максимальная температура (slider)
//V9-минимальная влажность (slider)
//V10-максимальная влажность(slider)
//**********************************************************
#define BLYNK_PRINT Serial
#include "DHT.h"
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "e4c7710ea10345059dc7ef625557f6dc";
char ssid[] = "Honor";//Dom_ru_TP-Link_8A80
char pass[] = "123321123";//romansolid123


#define DHTPIN 13          // Пин на DHT 11
#define DOORPIN 12         //Пин на серво дверь
#define WATERPIN 15        //Пин на серво воду
#define DHTTYPE DHT11     // DHT 11

Servo servo_DOOR;
Servo servo_WATER;
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
int analogPin = 0;
float t,min_t=20, max_t=40;
int val,min_val=40,max_val=70;
int AUTO_HAND=1;
int edge_W=0;
int signal_W=LOW;
int edge_D=85;
int signal_D=LOW;
//////////////////////////////////////
void sendSensor()
{
  float h = dht.readHumidity();
   t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
} 
void Vlag ()
{
    val = analogRead(analogPin);
    val = map(val,1023,0,0,100);
    Blynk.virtualWrite(V4, val);
}
void WaterVlag()
{
  if(val>max_val)
 {
  Blynk.virtualWrite (V1, HIGH);
  servo_WATER.write(80);
  edge_W=80;
  signal_W=HIGH;
 }
 else if(val<min_val)
 {
  Blynk.virtualWrite (V1, LOW);
  servo_WATER.write(0);
  edge_W=0;
  signal_W=LOW;
 }
 else
 {
  Blynk.virtualWrite (V1, signal_W);
  //servo_WATER.write(edge_W);
 }
}
void DverTem ()
{
  if(t>max_t)
 {
  Blynk.virtualWrite (V3, HIGH);
  servo_DOOR.write(0);
  edge_D=0;
  signal_D=HIGH;
 }
 else if(t<min_t)
 {
  Blynk.virtualWrite (V3, LOW);
  servo_DOOR.write(85);
  edge_D=85;
  signal_D=LOW;
 }
  else
 {
  Blynk.virtualWrite (V3, signal_D);
 // servo_DOOR.write(edge_D);
 }
}

///////////////////////////////////////////////////////////
BLYNK_WRITE(V1)
{
  if(!AUTO_HAND)
  {
  if(param.asInt()==0)
  {
     servo_WATER.write(0);
     signal_W=LOW;
  }
  else
  {
     servo_WATER.write(80);
     signal_W=HIGH;
  }
  }
}
BLYNK_WRITE(V7)
{
  min_t=param.asInt();
}
BLYNK_WRITE(V8)
{
  max_t=param.asInt();
}
BLYNK_WRITE(V9)
{
  min_val=param.asInt();
}
BLYNK_WRITE(V10)
{
  max_val=param.asInt();
}
BLYNK_WRITE(V3)
{
  if(!AUTO_HAND)
  {
  if(param.asInt()==0)
  {
     servo_DOOR.write(85);
     signal_D=LOW;
  }
  else
  {
     servo_DOOR.write(0);
     signal_D=HIGH;
  }
  }
}
BLYNK_WRITE(V2)
{
  if(param.asInt()==0)
    AUTO_HAND=1;
  else
    AUTO_HAND=0;
}
//////////////////////////////////////////////////
void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth,ssid,pass);
  dht.begin();
  servo_DOOR.attach(DOORPIN);
  servo_WATER.attach(WATERPIN);
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
  timer.setInterval(1000L, Vlag);
  Blynk.virtualWrite(V7, min_t);/////|установка изначальных показателей 
  Blynk.virtualWrite(V8, max_t);/////|влажности и температуры
  Blynk.virtualWrite(V9, min_val);///|
  Blynk.virtualWrite(V10, max_val);//|
}
void loop()
{
  Blynk.run();
  timer.run();
  if(AUTO_HAND)
  {
    DverTem();
    WaterVlag();
  }
}
