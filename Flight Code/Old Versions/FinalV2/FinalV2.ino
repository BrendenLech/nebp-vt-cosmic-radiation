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
#define geigerPin 1
#define millisecondsBetweenCountReports 60000-1000
#define SEALEVELPRESSURE_HPA (1017) //Change this value to the sea level pressure for current location during launch 
volatile int particleCount = 0;
  
#define USE_MPL 1
#define USE_RTC 1
#define USE_LOG 1
#define USE_SERIAL 1

#ifdef USE_MPL
Adafruit_MPL3115A2 mpl;
#endif
#ifdef USE_RTC
RTC_DS3231 rtc;
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
  #ifdef USE_Serial
    Serial.begin(9600);
    while (!Serial);

      if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      Serial.flush();
      while (1) delay(10);
    }
  #endif // USE_SERIAL

  /* Geiger Counter Setup */
  // Specifies the pin to use
  pinMode(geigerPin, INPUT);

  // Registers the particleDetected() function to run every time the pin goes to HIGH
  int geigerInterruptNumber = digitalPinToInterrupt(geigerPin);
  attachInterrupt(geigerInterruptNumber, particleDetected, RISING);

  /* MPL3115A2 setup */
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

  /* SD Card setup */
  #ifdef USE_LOG
  // See if the card is present and can be initialized
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
  strcpy(filename, "/GIEGER00.CSV");
  for (uint8_t i = 0; i < 100; i++)
  {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // Create if does not exist, do not open existing, write, sync after write
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

  logfile.print("Time");
  logfile.print(',');
  logfile.print("Counts");
  logfile.print(',');
  logfile.print("Altitude (m)");
  logfile.print(',');
  logfile.print("Pressure (hPa)");
  logfile.print(',');
  logfile.print("RTC Temperature (C)");
  logfile.print(',');
  logfile.print("MPL Temperature (C)");
  logfile.println();
  #endif // USE_LOG
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.println("Ready!");
}

void loop() {
  DateTime now = rtc.now();
  // Waits a certain number of ms before reporting the number of counts in that period
  delay(millisecondsBetweenCountReports);

  // Stops particleDetected() from running while the particle count amount is read and reset to 0
  noInterrupts();
  int countsSinceLastReport = particleCount;
  particleCount = 0;
  interrupts();

  // Reports the number of counts since the last report
  #ifdef USE_LOG
  #ifdef USE_MPL
  logfile.print(now.hour(), DEC);
  logfile.print(':');
  logfile.print(now.minute(), DEC);
  logfile.print(':');
  logfile.print(now.second(), DEC);
  logfile.print(',');
  logfile.print(countsSinceLastReport);
  logfile.print(',');
  logfile.print(mpl.getAltitude());
  logfile.print(',');
  logfile.print(mpl.getPressure());
  logfile.print(',');
  logfile.print(rtc.getTemperature());
  logfile.print(',');
  logfile.print(mpl.getTemperature());
  logfile.println();
  #endif // USE_MPL
  #endif // USE_Log

  /*Actually makes sure this logs data*/
  #ifdef USE_LOG
    logfile.flush();
  #endif // USE_LOG

  /*idr what this is for*/
  #ifdef USE_SERIAL
    Serial.println();
  #endif // USE_SERIAL
}

void particleDetected() {
  particleCount++;
}