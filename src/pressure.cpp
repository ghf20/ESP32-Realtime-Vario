#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include <Adafruit_BMP280.h>
#include "pressure.h"

#define ARRAY_LEN 1000
#define SEALEVEL 1006.00
#define ALPHA 0.8

Adafruit_BMP280 bmp280;

bool setupPressure(void)
{
    if (bmp280.begin(0x76)) {
        bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_OFF);
        return true;
    }
    return false;
}


float Pressure::getPressure(void)
{
    return bmp280.readAltitude(1024.0F);
}
