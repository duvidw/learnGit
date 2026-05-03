#include <Arduino.h>
#include <NimBLEDevice.h>

#include "Mpu6050.h"

namespace {
constexpr char kDeviceName[] = "ESP32S3-MPU6050";
constexpr char kServiceUuid[] = "6df0f54f-5a5f-4f53-a31d-2160c7d2f001";
constexpr char kRotationCharacteristicUuid[] = "6df0f54f-5a5f-4f53-a31d-2160c7d2f002";
constexpr uint32_t kPublishIntervalMs = 500;

Mpu6050 imu;
NimBLEServer* bleServer = nullptr;
NimBLECharacteristic* rotationCharacteristic = nullptr;
unsigned long lastPublishAt = 0;

String formatRotation(const Mpu6050::Rotation& rotation) {
  char payload[64];
  snprintf(payload,
           sizeof(payload),
           "x=%.2f,y=%.2f,z=%.2f",
           rotation.x,
           rotation.y,
           rotation.z);
  return String(payload);
}

void startBleServer() {
  NimBLEDevice::init(kDeviceName);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);

  bleServer = NimBLEDevice::createServer();
  NimBLEService* service = bleServer->createService(kServiceUuid);

  rotationCharacteristic = service->createCharacteristic(
      kRotationCharacteristicUuid,
      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  rotationCharacteristic->setValue("x=0.00,y=0.00,z=0.00");

  service->start();

  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  advertising->addServiceUUID(kServiceUuid);
  advertising->start();
}
}  // namespace

void setup() {
  Serial.begin(115200);
  delay(250);

  if (!imu.begin()) {
    Serial.println("MPU6050 initialization failed");
    while (true) {
      delay(1000);
    }
  }

  startBleServer();
  Serial.println("NimBLE MPU6050 server started");
}

void loop() {
  const unsigned long now = millis();
  if (now - lastPublishAt < kPublishIntervalMs) {
    delay(10);
    return;
  }

  lastPublishAt = now;

  const Mpu6050::Rotation rotation = imu.readRotation();
  const String payload = formatRotation(rotation);

  rotationCharacteristic->setValue(payload.c_str());
  rotationCharacteristic->notify();
  NimBLEDevice::getAdvertising()->setName(kDeviceName);

  Serial.println(payload);
}