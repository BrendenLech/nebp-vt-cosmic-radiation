#include <Wire.h>
#include <SD.h>
#include <Adafruit_MPL3115A2.h>
#include <RTClib.h>
#include <Adafruit_GPS.h>

#define MPL_SCK 13
#define MPL_MISO 12
#define MPL_MOSI 11
#define MPL_CS 10
#define cardSelect 4
#define ERROR_DISPLAY_PIN 13
#define geigerPin 1
#define millisecondsBetweenCountReports 1000
#define SEALEVELPRESSURE_HPA (1017) //Change this value to the sea level pressure for current location during launch 
volatile int particleCount = 0;

#define USE_MPL 1
#define USE_LOG 1
#define USE_RTC 1
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

// Connect to the GPS on the hardware I2C port
#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

void error(uint8_t errno) {
  while(1)   {
    uint8_t i;
    for (i = 0; i < errno; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
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

  // This line turns on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate

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
  strcpy(filename, "/TEST00.CSV");
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

  logfile.println(F("Time, Counts, MPL Altitude (m), GPS Altitude (m), Pressure (hPa), RTC Temperature (C), MPL Temperature (C)"));
  logfile.flush();
  #endif // USE_LOG
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  
  Serial.println("Ready!");
}

void loop() {

  delay(50);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);

  // Waits a certain number of ms before reporting the number of counts in that period
  delay(millisecondsBetweenCountReports);

  // Stops particleDetected() from running while the particle count amount is read and reset to 0
  noInterrupts();
  int countsSinceLastReport = particleCount;
  particleCount = 0;
  interrupts();
  
  // Reports the number of counts since the last report
  #ifdef USE_SERIAL
  #ifdef USE_LOG
  #ifdef USE_MPL
  DateTime now = rtc.now();
  Serial.print(F("Time: "));
  Serial.print(now.hour(), DEC);
  Serial.print(F(":"));
  Serial.print(now.minute(), DEC);
  Serial.print(F(":"));
  Serial.print(now.second(), DEC);
  
  logfile.print(now.hour(), DEC);
  logfile.print(F(":"));
  logfile.print(now.minute(), DEC);
  logfile.print(F(":"));
  logfile.print(now.second(), DEC);

  Serial.print(F(", CPS "));
  Serial.print(countsSinceLastReport);

  
  logfile.print(F(", "));
  logfile.print(countsSinceLastReport);

  Serial.print(F(", MPL Alt: "));
  Serial.print(mpl.getAltitude());
  
  logfile.print(F(", "));
  logfile.print(mpl.getAltitude());

  Serial.print(F(", GPS Alt: "));
  Serial.print(GPS.altitude);

  logfile.print(F(", "));
  logfile.print(GPS.altitude);

  Serial.print(F(", Pressure: "));
  Serial.print(mpl.getPressure());

  logfile.print(F(", "));
  logfile.print(mpl.getPressure());

  Serial.print(F(", RTC Temp: "));
  Serial.print(rtc.getTemperature());

  logfile.print(F(", "));
  logfile.print(rtc.getTemperature());

  Serial.print(F(", MPL Temp: "));
  Serial.print(mpl.getTemperature());
  Serial.println();

  logfile.print(F(", "));
  logfile.print(mpl.getTemperature());
  logfile.println();
  #endif // USE_MPL
  logfile.flush();
  #endif // USE_LOG
  #endif // USE_SERIAL
}

void particleDetected() {
  particleCount++;
}