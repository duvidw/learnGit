#pragma once

#include <Arduino.h>
#include <Adafruit_MPU6050.h>

class Mpu6050 {
public:
  struct Rotation {
    float x;
    float y;
    float z;
  };

  explicit Mpu6050(TwoWire& wirePort = Wire, uint8_t address = 0x68);

  bool begin();
  Rotation readRotation();

private:
  Adafruit_MPU6050 mpu;
  TwoWire* wire;
  uint8_t deviceAddress;
};