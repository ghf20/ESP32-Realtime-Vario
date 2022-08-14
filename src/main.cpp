#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "gyro.h"
#include "pressure.h"
#include <altitude.h>

#define Length 1000
const int buzzer = 19;
int current = 0;
Pressure pres;

static AltitudeEstimator altitude = AltitudeEstimator(0.0005, 	// sigma Accel
                                               0.005, 	// sigma Gyro
                                               0.0176,   // sigma Baro
                                               0.5, 	// ca
                                               0.1);	// accelThreshold

float pastTime = millis();
float currentTime = millis();

float average[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
int counter = 0;

float moving_average(float arg) {
  if (counter >= INT64_MAX) {
    counter=0;
  }
  average[++counter%5] = arg;
  float mean = 0;
  for (int i = 0; i < 5; i++) {
    mean += average[i];
  }
  return mean / 5;
}

void setup() {

  Serial.begin(9600);
  Serial.println("Initializing BMP280");
  bool bmp280Status = setupPressure();
  bool mpu_status = setupGyro();
  if (!bmp280Status || !mpu_status) {
    Serial.println("Not connected");
  }
  pinMode(buzzer, OUTPUT);
}


void loop() 
{
  currentTime = millis();
  if ((currentTime - pastTime) > 50) {
    float acceleration[3] = {0.0, 0.0, 0.0};
    float gyrometer[3] = {0.0, 0.0, 0.0};

    float baroHeight = pres.getPressure();
    getVals(acceleration, gyrometer);
    uint32_t timestamp = micros();


    
    altitude.estimate(acceleration, gyrometer, baroHeight, timestamp);
    Serial.print(moving_average(altitude.getVerticalVelocity()));
    Serial.print('\n');
    Serial.print(altitude.getAltitude());
    Serial.print("\n");
    Serial.print(altitude.getVerticalVelocity());
    Serial.print("\n");
    Serial.println(altitude.getVerticalAcceleration());
    Serial.print("\n");
    pastTime = currentTime;
  }
  

}