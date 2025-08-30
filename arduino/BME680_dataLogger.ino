/***************************************************************************
This program initializes the BME680 to begin taking temperature, pressure,
humidity, and gas resistance values and writes these values onto an SD card
(with a time stamp) into a text (.txt) file.
 ***************************************************************************/

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

#define SEALEVELPRESSURE_HPA (1013.25)

// Adafruit_BME680 bme; // I2C
Adafruit_BME680 bme(BME_CS); // hardware SPI
// Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

void setup()
{

  Serial.begin(9600);

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }

  if (!rtc.initialized())
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  if (!SD.begin(10))
  {
    Serial.println("SD card initialization failed.");
    while (1)
      ;
  }

  if (!bme.begin())
  {
    Serial.println("Could not find a valid BME680 sensor.");
    while (1)
      ;
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop()
{
  delay(300000);

  // Open myFile to write to it
  DateTime now = rtc.now();
  myFile = SD.open("test.txt", FILE_WRITE);

  if (!bme.performReading())
  {
    Serial.println("Failed to perform reading.");
    return;
  }

  if (myFile)
  {
    myFile.print(now.year(), DEC);
    myFile.print('-');
    myFile.print(now.month(), DEC);
    myFile.print('-');
    myFile.print(now.day(), DEC);
    myFile.print(" ");
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.print(",");
    myFile.print(bme.temperature);
    myFile.print(",");
    myFile.print(bme.pressure / 100.0);
    myFile.print(",");
    myFile.print(bme.humidity);
    myFile.print(",");
    myFile.println(bme.gas_resistance / 1000.0);
    myFile.close();
    Serial.print("Data Logged ");
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC);
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("Error opening file.");
    while (1)
      ;
  }
}
