/*
    altitude.h: Altitude estimation via barometer/accelerometer fusion
*/

# pragma once

#include "filters.h"
#include "algebra.h"

#include <Arduino.h> // XXX For micros; eventually need to compute micros() elsewhere

class AltitudeEstimator {

  private:
    // required parameters for the filters used for the estimations
    // sensor's standard deviations
    float sigmaAccel;
    float sigmaGyro;
    float sigmaBaro;
    // Acceleration markov chain model state transition constant
    float ca;
    // Zero-velocity update acceleration threshold
    float accelThreshold;
    // gravity
    float g = 9.81;
    // For computing the sampling period
    uint32_t previousTime = micros();
    // required filters for altitude and vertical velocity estimation
    KalmanFilter kalman;
    ComplementaryFilter complementary;
    // Estimated past vertical acceleration
    float pastVerticalAccel = 0;
    float pastVerticalVelocity = 0;
    float pastAltitude = 0;
    float pastGyro[3] = {0, 0, 0};
    float pastAccel[3] = {0, 0, 0};
    // estimated altitude and vertical velocity
    float estimatedAltitude = 0;
    float estimatedVelocity = 0;

  public:

    AltitudeEstimator(float sigmaAccel, float sigmaGyro, float sigmaBaro,
                      float ca, float accelThreshold);

    void estimate(float accel[3], float gyro[3], float baroHeight, uint32_t timestamp);

    float getAltitude();

    float getVerticalVelocity();

    float getVerticalAcceleration();

}; // class AltitudeEstimator
