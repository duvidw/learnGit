#include "Mpu6050.h"

Mpu6050::Mpu6050(TwoWire& wirePort, uint8_t address)
    : wire(wirePort), deviceAddress(address) {}

bool Mpu6050::begin() {
  wire.begin();

  if (readByte(Register::WhoAmI) != 0x68) {
    return false;
  }

  writeByte(Register::PowerManagement1, 0x00);
  delay(100);

  writeByte(Register::SampleRateDivider, 0x07);
  writeByte(Register::Configuration, 0x00);
  writeByte(Register::GyroConfiguration, 0x00);
  return true;
}

Mpu6050::Rotation Mpu6050::readRotation() {
  uint8_t rawData[6] = {0};
  readBytes(Register::GyroXOutHigh, rawData, sizeof(rawData));

  const int16_t rawX = static_cast<int16_t>((rawData[0] << 8) | rawData[1]);
  const int16_t rawY = static_cast<int16_t>((rawData[2] << 8) | rawData[3]);
  const int16_t rawZ = static_cast<int16_t>((rawData[4] << 8) | rawData[5]);

  return Rotation{
      rawX / GyroScaleDegreesPerSecond,
      rawY / GyroScaleDegreesPerSecond,
      rawZ / GyroScaleDegreesPerSecond,
  };
}

void Mpu6050::writeByte(uint8_t reg, uint8_t value) {
  wire.beginTransmission(deviceAddress);
  wire.write(reg);
  wire.write(value);
  wire.endTransmission();
}

uint8_t Mpu6050::readByte(uint8_t reg) {
  uint8_t value = 0;
  readBytes(reg, &value, 1);
  return value;
}

void Mpu6050::readBytes(uint8_t reg, uint8_t* buffer, size_t length) {
  wire.beginTransmission(deviceAddress);
  wire.write(reg);
  wire.endTransmission(false);
  wire.requestFrom(static_cast<int>(deviceAddress), static_cast<int>(length));

  size_t index = 0;
  while (wire.available() && index < length) {
    buffer[index++] = static_cast<uint8_t>(wire.read());
  }
}