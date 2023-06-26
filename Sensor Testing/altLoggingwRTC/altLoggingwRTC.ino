#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_MPL3115A2.h>
#include <RTClib.h>

#define MPL_SCK 13
#define MPL_MISO 12
#define MPL_MOSI 11
#define MPL_CS 10
#define cardSelect 4
#define ERROR_DISPLAY_PIN 13
#define SEALEVELPRESSURE_HPA (1014) //Change this value to the sea level pressure for current location during launch 

#define USE_MPL 1
#define USE_LOG 1
#define USE_SERIAL 1 //Comment this out for flight to save memory

#ifdef USE_MPL
Adafruit_MPL3115A2 mpl;
#endif

#ifdef USE_RTC
RTC_PCF8523 rtc;
#endif

#ifdef USE_LOG
File logfile;
#endif

void error(uint8_t errno) {
  while(1)   {
    uint8_t i;
    for (i = 0; i < errno; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
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
  #endif // USE_MPL

  /* This checks if the RTC is actually running lol */
  #ifdef USE_RTC
  if (! rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // following line sets the RTC to the date & time this sketch was compiled using YOUR computers date and time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  #endif // USE_RTC

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
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.println("Ready!");
}

void loop() {
  #if USE_SERIAL
    #if USE_RTC
    DateTime now = rtc.now();
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print('/');    
    Serial.print(now.year(), DEC);
    Serial.print(',');    
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    #endif
  #endif
  
  /*Collects Pressure and Altitude from the MPL3115A2*/
  #ifdef USE_MPL
  /*Logs the Pressure*/
  #if USE_SERIAL
    Serial.print("Pressure: ");
    Serial.print(mpl.getPressure());
    Serial.println(" hPa");
  #endif // USE_SERIAL
  #ifdef USE_LOG
    logfile.print("Pressure, "); logfile.println(mpl.getPressure());  
  #endif // USE_LOG
  /*Logs the altitude*/
  #if USE_SERIAL
    Serial.print("Approx. Altitude = ");
    Serial.print(mpl.getAltitude());
    Serial.println(" m");
  #endif // USE_SERIAL
  #ifdef USE_LOG
    logfile.print("Altitude, "); logfile.println(mpl.getAltitude());    
  #endif // USE_LOG
  #endif // USE_MPL

  #ifdef USE_LOG
  #ifdef USE_RTC
  #ifdef USE_MPL
    DateTime now = rtc.now(); 
    logfile.print(now.month(), DEC);logfile.print('/'); logfile.print(now.day(), DEC); logfile.print('/'); logfile.print(now.year(), DEC); logfile.print(','); logfile.print(now.hour(), DEC); logfile.print(':'); logfile.print(now.minute(), DEC); logfile.print(':'); logfile.print(now.second(), DEC); logfile.println(',');  logfile.println(mpl.getPressure()); logfile.println(mpl.getAltitude());   
  #endif
  #endif
  #endif

  /*Actually makes sure this logs data*/
  #ifdef USE_LOG
    logfile.flush();
  #endif // USE_LOG

  /*idr what this is for*/
  #ifdef USE_SERIAL
    Serial.println();
  #endif // USE_SERIAL

    delay(2000);
}
