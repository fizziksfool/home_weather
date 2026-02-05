#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <SD.h>
#include "RTClib.h"

RTC_PCF8523 rtc;
File myFile;

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 9

Adafruit_BME680 bme(BME_CS); // hardware SPI

void setup()
{
  delay(5000);
  Serial.begin(9600);
  Serial.println(F("Start"));

  // RTC init
  if (!rtc.begin())
  {
    Serial.println(F("No RTC"));
    while (1)
      ;
  }

  // SD init
  if (!SD.begin(10))
  {
    Serial.println(F("SD fail"));
    while (1)
      ;
  }
  Serial.println(F("SD ok"));

  // BME init
  if (!bme.begin())
  {
    Serial.println(F("No BME680"));
    while (1)
      ;
  }
  Serial.println(F("BME ok"));

  // Configure BME
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);

  // Print current RTC time for verification
  DateTime now = rtc.now();
  Serial.print(F("RTC time: "));
  Serial.print(now.year());
  Serial.print("-");
  Serial.print(now.month());
  Serial.print("-");
  Serial.print(now.day());
  Serial.print(" ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());
}

void loop()
{
  DateTime now = rtc.now();

  if (!SD.exists("test.csv"))
  {
    myFile = SD.open("test.csv", FILE_WRITE);
    if (myFile)
    {
      myFile.println(F("Time,Temperature,Pressure,Humidity,Gas"));
      myFile.close();
      Serial.println(F("Hdr OK"));
    }
    else
    {
      Serial.println(F("Hdr fail"));
    }
  }

  if (!bme.performReading())
  {
    Serial.println(F("BME fail"));
    return;
  }

  char ts[20];
  snprintf(ts, sizeof(ts), "%04d-%02d-%02d %02d:%02d:%02d",
           now.year(), now.month(), now.day(),
           now.hour(), now.minute(), now.second());

  myFile = SD.open("test.csv", FILE_WRITE);
  if (myFile)
  {
    myFile.print(ts);
    myFile.print(",");
    myFile.print(bme.temperature);
    myFile.print(",");
    myFile.print(bme.pressure / 100.0);
    myFile.print(",");
    myFile.print(bme.humidity);
    myFile.print(",");
    myFile.println(bme.gas_resistance / 1000.0);
    myFile.close();
    Serial.println(F("W ok"));
  }
  else
  {
    Serial.println(F("W fail"));
  }
  delay(300000);
}
