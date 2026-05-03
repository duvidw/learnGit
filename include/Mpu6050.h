#pragma once

#include <Arduino.h>
#include <Wire.h>

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
  enum Register : uint8_t {
    SampleRateDivider = 0x19,
    Configuration = 0x1A,
    GyroConfiguration = 0x1B,
    GyroXOutHigh = 0x43,
    PowerManagement1 = 0x6B,
    WhoAmI = 0x75,
  };

  static constexpr float GyroScaleDegreesPerSecond = 131.0f;

  void writeByte(uint8_t reg, uint8_t value);
  uint8_t readByte(uint8_t reg);
  void readBytes(uint8_t reg, uint8_t* buffer, size_t length);

  TwoWire& wire;
  uint8_t deviceAddress;
};