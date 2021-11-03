//SD card
#include <SPI.h>
#include <SD.h>
File myFile;
int newHour = 0; int oldHour = 0;

//time
#include <Wire.h> //library for I2C communication, also included later
#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

//temperature
#include <OneWire.h>
#include <DallasTemperature.h>
const int tempPin = 4;
OneWire temp(tempPin);
DallasTemperature sensor_temp(&temp);
float T;

//relative humidity
#include <DHT.h>
const int dhtPin = 7;
#define DHT_TYPE DHT11
DHT dht(dhtPin, DHT_TYPE);
float RH;

//soil moisture, resistive
const int sensorResPin = A0;
const int sensorResVCCPin = 6;
int sensorResValue; int sensorResPercent; 

//soil moisture, capacitive
const int sensorCapPin = A1;
int sensorCapValue; int sensorCapPercent;

void save_data(DateTime now) {
  Serial.println("Getting data...");
  
  //now = rtc.now(); //read time from RTC
  
  sensor_temp.requestTemperatures();
  T = sensor_temp.getTempCByIndex(0); //measure temperature
  
  RH = dht.readHumidity(); //measure relative humidity
  
  digitalWrite(sensorResVCCPin, HIGH);
  delay(2000);
  sensorResValue = analogRead(sensorResPin);
  //sensorResPercent = map(sensorResValue, 139, 125, 0, 100);
  delay(100);
  digitalWrite(sensorResVCCPin, LOW);

  sensorCapValue = analogRead(sensorCapPin);
  //sensorCapPercent = map(sensorCapValue, 502, 280, 0, 100);

  /*int x = 14;
  int y = 126;
  int z = 1007;
  char tbs[16];
  sprintf(tbs, "P%4dR%4dT%4d", x, y, z);
  Serial.print(tbs);*/

  Serial.println(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + ". " + String(now.hour()) + ":" + String(now.minute()));
  Serial.print(T, 1); Serial.println(" °C");
  Serial.print(RH, 1); Serial.println(" %");
  Serial.print(sensorResValue); Serial.println(" res");
  //Serial.print(sensorResPercent); Serial.println(" % res");
  Serial.print(sensorCapValue); Serial.println(" cap");
  //Serial.print(sensorCapPercent); Serial.println(" % cap");
  //Serial.println("");
  
  myFile = SD.open("data0000.txt", FILE_WRITE);
  myFile.print(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + ". " + String(now.hour()) + ":" + String(now.minute())); myFile.print(";");
  myFile.print(T, 1); myFile.print(";");
  myFile.print(RH, 1); myFile.print(";");
  myFile.print(sensorResValue); myFile.print(";");
  //myFile.print(sensorResPercent); myFile.print(";");
  myFile.print(sensorCapValue); myFile.println(";");
  //myFile.println(sensorCapPercent);
  myFile.close();
}

void setup ()
{
  Serial.println("Initialization...");
  Wire.begin();
  Serial.begin(9600);
  sensor_temp.begin();
  dht.begin();
  rtc.begin();
  Serial.print("SD card... ");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("Initialization done");
  delay(1000);
  Serial.println("Starting measurements\n");

  now = rtc.now(); oldHour = now.hour();
  save_data(now);
  Serial.println("Data saved at " + String(oldHour) + " hour"); Serial.println("");
  myFile = SD.open("data0000.txt", FILE_WRITE);
  myFile.println();
  myFile.close();
}

void loop ()
{
  now = rtc.now(); newHour = now.hour();
  if (oldHour != newHour) {
    save_data(now);
    Serial.println("Data saved at " + String(newHour) + " hour"); Serial.println("");
    oldHour = newHour;
  }
  /*save_data(now);
  Serial.println("Data saved at " + String(newHour) + " hour");
  Serial.println();
  delay(300000); //in milliseconds*/
}
