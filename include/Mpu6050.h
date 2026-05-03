#pragma once

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>

class Mpu6050 {
public:
  struct Angles {
    float pitch;
    float roll;
    float yaw;
  };

  explicit Mpu6050(TwoWire& wirePort = Wire, uint8_t address = 0x68);

  bool begin();
  Angles readAngles();

private:
  Adafruit_MPU6050 mpu;
  TwoWire* wire;
  uint8_t deviceAddress;
  float yaw = 0.0f;
  unsigned long lastReadTime = 0;
};