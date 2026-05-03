#include "Mpu6050.h"

Mpu6050::Mpu6050(TwoWire& wirePort, uint8_t address)
    : wire(&wirePort), deviceAddress(address) {}

bool Mpu6050::begin() {
  if (!mpu.begin(deviceAddress, wire)) {
    return false;
  }

  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  return true;
}

Mpu6050::Rotation Mpu6050::readRotation() {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  mpu.getEvent(&accel, &gyro, &temp);

  return Rotation{
      gyro.gyro.x * 57.2958f,
      gyro.gyro.y * 57.2958f,
      gyro.gyro.z * 57.2958f,
  };
}