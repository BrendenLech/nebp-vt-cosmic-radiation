#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_MPL3115A2.h>
#include "Adafruit_MPL3115A2.h"

#define MPL_SCK 13
#define MPL_MISO 12
#define MPL_MOSI 11
#define MPL_CS 10
#define cardSelect 4
#define SEALEVELPRESSURE_HPA (1013.25) //Change this value to the sea level pressure for current location during launch 

#define ERROR_DISPLAY_PIN 13

#define USE_MPL 1
#define USE_LOG 1
#define USE_SERIAL 1 //Comment this out for flight to save memory

#ifdef USE_MPL
Adafruit_MPL3115A2 mpl;
#endif

#ifdef USE_LOG
File logfile;
#endif

void error(uint8_t errno) {
  while(1)   {
    uint8_t i;
    for (i = 0; i < errno; i++) {
      digitalWrite(ERROR_DISPLAY_PIN, HIGH);
      delay(100);
      digitalWrite(ERROR_DISPLAY_PIN, LOW);
      delay(100);
    }
    for (i = errno; i < 10; i++) {
      delay(200);
    }
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  /* This checks if the MPL3115A2 sensor is actually connected lol */
  #ifdef USE_MPL  
    Serial.println(F("MPL3115A2 test"));

    if (!mpl.begin()) {
      Serial.println("Could not find a valid MPL3115A2 sensor (╯°□°)╯︵ ┻━┻, check wiring!");
      error (1);
    }
  #ifdef USE_SERIAL
    else {
      Serial.println("Found MPL3115A2 sensor");
    }
  #endif // USE_SERIAL
  #endif // USE_BME
  /* This creates a file in the SD card to write too */
  #ifdef USE_LOG
  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect))
  {
    Serial.println("Card init. failed! Check if you put a SD card in :/");
    error(2);
  }
  else
  {
    Serial.println("Card init. successfull!");
  }
  char filename[15];
  strcpy(filename, "/ANALOG00.TXT");
  for (uint8_t i = 0; i < 100; i++)
  {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (!SD.exists(filename))
    {
      break;
    }
  }
    logfile = SD.open(filename, FILE_WRITE);
  if (!logfile)
  {
    Serial.print("Couldn't create :/, check if there is free space on the SD card"); 
    Serial.println(filename);
    error(3);
  }
  Serial.print("Writing to "); 
  Serial.println(filename);
  #endif // USE_LOG
  pinMode(ERROR_PIN, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.println("Ready!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
