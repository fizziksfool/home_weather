#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <SD.h>
#include "RTClib.h"
#include <EEPROM.h>

RTC_PCF8523 rtc;
File myFile;

// --- Build signature for detecting new uploads ---
const char buildSignature[] = __DATE__ __TIME__;

// EEPROM constants
const int EEPROM_MAGIC_ADDR = 0; // start address
const byte EEPROM_MAGIC_VALUE = 0xA5; // used to check initialization
const int EEPROM_SIG_ADDR = 1;  // where the signature starts

// BME680 SPI pins
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 9

Adafruit_BME680 bme(BME_CS); // hardware SPI

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Start"));

  // --- RTC init ---
  if (!rtc.begin())
  {
    Serial.println(F("No RTC"));
    while (1);
  }

  // --- EEPROM signature check ---
  bool needsAdjust = false;

  // Check if EEPROM has been initialized before
  byte magic = EEPROM.read(EEPROM_MAGIC_ADDR);
  char savedSignature[sizeof(buildSignature)];

  if (magic != EEPROM_MAGIC_VALUE)
  {
    Serial.println(F("EEPROM uninitialized – syncing RTC"));
    needsAdjust = true;
  }
  else
  {
    for (unsigned int i = 0; i < sizeof(buildSignature); i++)
      savedSignature[i] = EEPROM.read(EEPROM_SIG_ADDR + i);

    if (memcmp(savedSignature, buildSignature, sizeof(buildSignature)) != 0)
    {
      Serial.println(F("New build detected – syncing RTC"));
      needsAdjust = true;
    }
    else
    {
      Serial.println(F("Same build – keeping RTC time"));
    }
  }

  // --- Adjust RTC if needed ---
  if (needsAdjust)
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
    for (unsigned int i = 0; i < sizeof(buildSignature); i++)
      EEPROM.write(EEPROM_SIG_ADDR + i, buildSignature[i]);
  }

  // --- SD init ---
  if (!SD.begin(10))
  {
    Serial.println(F("SD fail"));
    while (1);
  }
  Serial.println(F("SD ok"));

  // --- BME680 init ---
  if (!bme.begin())
  {
    Serial.println(F("No BME680"));
    while (1);
  }
  Serial.println(F("BME ok"));

  // --- BME configuration ---
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
  Serial.println(F("Loop"));

  DateTime now = rtc.now();

  // Create CSV file with header if not already present
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

  // Perform BME680 reading
  if (!bme.performReading())
  {
    Serial.println(F("BME fail"));
    return;
  }

  // Format timestamp
  char ts[20];
  snprintf(ts, sizeof(ts), "%04d-%02d-%02d %02d:%02d:%02d",
           now.year(), now.month(), now.day(),
           now.hour(), now.minute(), now.second());

  // Write data to SD
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
