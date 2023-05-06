#include <SPI.h>
#include <SD.h>

#include <Adafruit_MPL3115A2.h>

Adafruit_MPL3115A2 mpl;
// Set the pins used
#define cardSelect 4

File logfile;

// blink out an error code
void error(uint8_t errno) {
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }
}

// This line is not needed if you have Adafruit SAMD board package 1.6.2+
//   #define Serial SerialUSB

void setup() {
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("\r\nAnalog logger test");
  pinMode(13, OUTPUT);


  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
    error(2);
  }
  char filename[15];
  strcpy(filename, "/ANALOG00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }

  
  while(!Serial);
  Serial.println("Adafruit_MPL3115A2 test!");

  if (!mpl.begin()) {
    Serial.println("Could not find sensor. Check wiring.");
    while(1);
  }

  // set mode before starting a conversion
  Serial.println("Setting mode to barometer (pressure).");
  mpl.setMode(MPL3115A2_BAROMETER);
  }

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    error(3);
  }
  Serial.print("Writing to "); 
  Serial.println(filename);

  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.println("Ready!");
}

uint8_t i=0;
void loop() {
  // start a conversion
  Serial.println("Starting a conversion.");
  mpl.startOneShot();

  // do something else while waiting
  Serial.println("Counting number while calculating...");
  int count = 0;
  while (!mpl.conversionComplete()) {
    count++;
  }
  Serial.print("Done! Counted to "); Serial.println(count);

  // now get results

  digitalWrite(8, HIGH);
  Serial.print("Pressure = "); Serial.println(mpl.getLastConversionResults(MPL3115A2_PRESSURE)); 
  logfile.print("Pressure = "); logfile.println(mpl.getLastConversionResults(MPL3115A2_PRESSURE)); 
  Serial.print("Temperature = "); Serial.println(mpl.getLastConversionResults(MPL3115A2_TEMPERATURE));
  logfile.print("Temperature = "); logfile.println(mpl.getLastConversionResults(MPL3115A2_TEMPERATURE));
  Serial.println();
  digitalWrite(8, LOW);

  delay(1000);
}