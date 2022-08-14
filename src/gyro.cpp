#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <math.h>
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

void getVals(float acceleration[], float gyrometer[])
{
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    
    // Copy gyro values back out in rad/sec
    gyrometer[0] = g.gyro.x * M_PI / 180.0f;
    gyrometer[1] = g.gyro.y * M_PI / 180.0f;
    gyrometer[2] = g.gyro.z * M_PI / 180.0f;
  
    acceleration[0] = -a.acceleration.x;
    acceleration[1] = -a.acceleration.y;
    acceleration[2] = -a.acceleration.z;

    
}