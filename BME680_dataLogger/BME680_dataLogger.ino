#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <SD.h>
#include "RTClib.h"
#include <EEPROM.h>

RTC_PCF8523 rtc;
File myFile;

// --- Build signature to detect new uploads ---
const char buildSignature[] = __DATE__ __TIME__;

// BME680 SPI pin definitions
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

  // --- Detect new upload via EEPROM signature ---
  bool needsAdjust = false;
  char savedSignature[sizeof(buildSignature)];

  for (unsigned int i = 0; i < sizeof(buildSignature); i++)
    savedSignature[i] = EEPROM.read(i);

  if (memcmp(savedSignature, buildSignature, sizeof(buildSignature)) != 0)
  {
    needsAdjust = true;
    Serial.println(F("New build detected – syncing RTC"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    for (unsigned int i = 0; i < sizeof(buildSignature); i++)
      EEPROM.write(i, buildSignature[i]);
  }
  else
  {
    Serial.println(F("Same build – keeping RTC time"));
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

  delay(1000);
}
