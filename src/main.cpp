#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "gyro.h"
#include "pressure.h"
#include <altitude.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <string>

#define BUZZER 19
#define MOVING_AVERAGE 10
#define AXIS 3
#define ACCEL_SIGMA 0.0005
#define GYRO_SIGMA 0.005
#define BARO_SIGMA 0.0176
#define CA 0.5
#define ACCEL_THRESHOLD 0.1
#define BAUD 9600
#define SAMPLING_RATE 25 // 20HZ
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
  

int current = 0;
Pressure pres;
float pastTime = millis();
float currentTime = millis();
bool deviceConnected = false;
bool oldDeviceConnected = false;
float pastTimeBLE = millis();
float currentTimeBLE = millis();

float average[MOVING_AVERAGE] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
int counter = 0;
float velocity = 9;
float pressure = 9999;
BLEServer* pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
static AltitudeEstimator altitude = AltitudeEstimator(ACCEL_SIGMA,
                                               GYRO_SIGMA,
                                               BARO_SIGMA,
                                               CA,
                                               ACCEL_SIGMA);


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


uint8_t checkSum(String checkString)
{
  uint8_t checksum = 0;
  for (int i = 1; i < checkString.length() -1; i++) {
    checksum ^= checkString[i];
  }

  return checksum;
}

String lk8ex1String(void)
{
  // $LK8EX1,pressure,altitude,vario,temperature,battery,*checksum

  String protocol = "$LK8EX1,";
  String presString = String((int)pressure);
  String varioString = String((int)velocity*100);
  String outputstring = protocol + presString + "," + "99999," + varioString + "," + "99," + "900,*";
  uint8_t checksum = checkSum(outputstring);
  outputstring += String(checksum);
  return outputstring;

}

void ble_connection(void)
{
  Serial.print("Child");
  //Setup
  BLEDevice::init("ESP32 Vario");
  
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                      CHARACTERISTIC_UUID,
                                      BLECharacteristic::PROPERTY_READ |
                                      BLECharacteristic::PROPERTY_WRITE |
                                       BLECharacteristic::PROPERTY_NOTIFY);
  String toSend = lk8ex1String();
  pCharacteristic->setValue(toSend.c_str());
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

}

float moving_average(float arg) {

  if (counter >= INT64_MAX) {
    counter=0;
  }
  average[++counter%MOVING_AVERAGE] = arg;
  float mean = 0;
  for (int i = 0; i < MOVING_AVERAGE; i++) {
    mean += average[i];
  }
  return mean / MOVING_AVERAGE;
}

void setup() {

  Serial.begin(BAUD);
  Serial.println("Initializing BMP280");
  bool bmp280Status = setupPressure();
  bool mpu_status = setupGyro();
  if (!bmp280Status || !mpu_status) {
    Serial.println("Not connected");
  }
  pinMode(BUZZER, OUTPUT);
  ble_connection();

}


void loop() 
{
  
  currentTime = millis();
  currentTimeBLE = millis();
  if ((currentTime - pastTime) > SAMPLING_RATE) {
    float acceleration[AXIS] = {0.0, 0.0, 0.0};
    float gyrometer[AXIS] = {0.0, 0.0, 0.0};

    float baroHeight = pres.getHeight();
    getVals(acceleration, gyrometer);
    uint32_t timestamp = micros();
    
    altitude.estimate(acceleration, gyrometer, baroHeight, timestamp);
    velocity = moving_average(altitude.getVerticalVelocity());
    // velocity = altitude.getVerticalVelocity();
    pressure = pres.getPressure();
    pastTime = currentTime;
  }
  // Only Send Bluetooth if connected
  if (deviceConnected) {
      if ((currentTimeBLE - pastTimeBLE) > 200) {
      String toSend = lk8ex1String();
      Serial.print(velocity);
      Serial.print("\n");
      pCharacteristic->setValue(toSend.c_str());
      pCharacteristic->notify();
      pastTimeBLE = currentTimeBLE;
    }
  }
  // No device connected
  if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
  if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
  }
  
  

}