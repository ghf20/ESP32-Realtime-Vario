#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include "gyro.h"

Adafruit_MPU6050 mpu;

bool setupGyro(void)
{
    if (mpu.begin(0x68)) {
        mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
        mpu.setMotionDetectionThreshold(1);
        mpu.setMotionDetectionDuration(20);
        mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
        mpu.setInterruptPinPolarity(true);
        mpu.setMotionInterrupt(true);
        return true;
    }
    return false;
}

static void printVals()
{
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    /* Print out the values */
    Serial.print("AccelX:");
    Serial.print(a.acceleration.x);
    Serial.print(",");
    Serial.print("AccelY:");
    Serial.print(a.acceleration.y);
    Serial.print(",");
    Serial.print("AccelZ:");
    Serial.print(a.acceleration.z);
    Serial.print(", ");
    Serial.print("GyroX:");
    Serial.print(g.gyro.x);
    Serial.print(",");
    Serial.print("GyroY:");
    Serial.print(g.gyro.y);
    Serial.print(",");
    Serial.print("GyroZ:");
    Serial.print(g.gyro.z);
    Serial.println("");
}

void printGyro()
{
    if(mpu.getMotionInterruptStatus()) {
    /* Get new sensor events with the readings */
    printVals();
  }
}