// Taken from https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management
#define VBATPIN A7

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // Read analog voltage from VBATPIN
  float measuredvbat = analogRead(VBATPIN);
  
  // Convert analog reading to voltage
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  
  // Print the measured voltage
  Serial.print("VBat: ");
  Serial.println(measuredvbat);
  
  delay(1000); // Delay for 1 second
}
