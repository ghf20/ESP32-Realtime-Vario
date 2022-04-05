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
                    Adafruit_BMP280::FILTER_X16);
        return true;
    }
    return false;
}

void printPressure(void)
{
    // float temp = bmp280.readTemperature();
    float press = bmp280.readPressure() / 100;
    Serial.println("Pressure:");
    Serial.println(press);
}

void Pressure::addToPressure(int ind)
{
    float pressure = bmp280.readPressure();
    float temp = bmp280.readTemperature();
    float height = bmp280.readAltitude(SEALEVEL);
    Pressure::pressure[ind] = pressure;
    Pressure::temp[ind] = temp;
    Pressure::height[ind] = height;
}

float Pressure::updatePressure(int startInd, int stopInd)
{
    int i = (startInd % ARRAY_LEN);
    float baseVal = Pressure::pressure[startInd%ARRAY_LEN];
    float startHeight = Pressure::height[startInd%ARRAY_LEN];
    //Serial.println(startHeight);
    float avgHeight = 0;
    float avgPressure = 0;
    int ind = 0; 
    do
    {
        
        avgHeight += (pow(1-ALPHA, ind)) * (startHeight - Pressure::height[i]);
        avgPressure += (baseVal - Pressure::pressure[i]);
        baseVal = Pressure::pressure[i];
        startHeight = Pressure::height[i++];
        i = (i + 1) % ARRAY_LEN;
    } while (i != stopInd%ARRAY_LEN);

    return -ALPHA*avgHeight;
}