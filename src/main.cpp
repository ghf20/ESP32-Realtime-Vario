#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "gyro.h"
#include "pressure.h"

#define Length 1000
const int buzzer = 19;
int current = 0;
Pressure pres;

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing BMP280");
  bool bmp280Status = setupPressure();
  bool mpu_status = setupGyro();
  if (!bmp280Status || !mpu_status) {
    Serial.println("Not connected");
  }
  pinMode(buzzer, OUTPUT);
  // create a name for the file output
}
void loop() {
  pres.addToPressure(current++%Length);
  //printPressure();
  if (current > 100 && current % 10 == 0) {
    float output = pres.updatePressure(current-100, current);
    Serial.println(output, 5);
    if (output >= 0.05) {
      digitalWrite(buzzer, HIGH);
      delay(250);
      digitalWrite(buzzer, LOW);
    }
  }
}
