#include "Mpu6050.h"

#include <math.h>

Mpu6050::Mpu6050(TwoWire& wirePort, uint8_t address)
    : wire(&wirePort), deviceAddress(address) {}

bool Mpu6050::begin() {
  if (!mpu.begin(deviceAddress, wire)) {
    return false;
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  return true;
}

Mpu6050::Angles Mpu6050::readAngles() {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  mpu.getEvent(&accel, &gyro, &temp);

  const float ax = accel.acceleration.x;
  const float ay = accel.acceleration.y;
  const float az = accel.acceleration.z;

  const float pitch = atan2f(ax, sqrtf((ay * ay) + (az * az))) * 57.2958f;
  const float roll = atan2f(ay, sqrtf((ax * ax) + (az * az))) * 57.2958f;

  return Angles{
      pitch,
      roll,
  };
}