#include <Wire.h>
#define geigerPin 1
#define millisecondsBetweenCountReports 6000 - 1000

void setup() {
}

void loop() {
    delay(millisecondsBetweenCountReports);

  // Stops particleDetected() from running while the particle count amount is read and reset to 0
  noInterrupts();
  int countsSinceLastReport = particleCount;
  particleCount = 0;
  interrupts();

  Serial.print(F("CPS:"));
  Serial.print(countsSinceLastReport);
}

void particleDetected() {
  particleCount++;
}