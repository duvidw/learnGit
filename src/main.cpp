#include <Arduino.h>
#include <NimBLEDevice.h>
#include <Wire.h>

#include "Mpu6050.h"

namespace {
constexpr char kDeviceName[] = "ESP32-MPU6050";
constexpr char kServiceUuid[] = "6df0f54f-5a5f-4f53-a31d-2160c7d2f001";
constexpr char kRotationCharacteristicUuid[] = "6df0f54f-5a5f-4f53-a31d-2160c7d2f002";
constexpr uint32_t kPublishIntervalMs = 500;
constexpr uint32_t kImuRetryIntervalMs = 2000;

#if defined(CONFIG_IDF_TARGET_ESP32C3)
constexpr int kI2cSdaPin = 5; //8;
constexpr int kI2cSclPin = 6; //9;
#else
constexpr int kI2cSdaPin = 21;
constexpr int kI2cSclPin = 22;
#endif

Mpu6050 imu;
NimBLEServer* bleServer = nullptr;
NimBLECharacteristic* rotationCharacteristic = nullptr;
bool bleClientConnected = false;
unsigned long lastPublishAt = 0;
unsigned long lastImuRetryAt = 0;
bool imuReady = false;

class ServerCallbacks : public NimBLEServerCallbacks {
public:
  void onConnect(NimBLEServer* server, NimBLEConnInfo& connInfo) override {
    bleClientConnected = true;
    Serial.printf("BLE client connected: %s\n", connInfo.getAddress().toString().c_str());
  }

  void onDisconnect(NimBLEServer* server, NimBLEConnInfo& connInfo, int reason) override {
    bleClientConnected = false;
    Serial.printf("BLE client disconnected (reason=%d), restarting advertising\n", reason);
    NimBLEDevice::startAdvertising();
  }
};

ServerCallbacks serverCallbacks;

String formatAngles(const Mpu6050::Angles& angles) {
  char payload[64];
  snprintf(payload,
           sizeof(payload),
           "pitch=%.2f,roll=%.2f,yaw=%.2f",
           angles.pitch,
           angles.roll,
           angles.yaw);
  return String(payload);
}

void startBleServer() {
  NimBLEDevice::init(kDeviceName);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);

  bleServer = NimBLEDevice::createServer();
  if (bleServer == nullptr) {
    Serial.println("Failed to create BLE server");
    return;
  }

  bleServer->setCallbacks(&serverCallbacks);
  NimBLEService* service = bleServer->createService(kServiceUuid);
  if (service == nullptr) {
    Serial.println("Failed to create BLE service");
    return;
  }

  rotationCharacteristic = service->createCharacteristic(
      kRotationCharacteristicUuid,
      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  if (rotationCharacteristic == nullptr) {
    Serial.println("Failed to create BLE characteristic");
    return;
  }
  rotationCharacteristic->setValue("pitch=0.00,roll=0.00,yaw=0.00");

  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  advertising->addServiceUUID(kServiceUuid);
  advertising->setName("C3-MPU6050");
  advertising->start();
}
}  // namespace

void setup() {
  Serial.begin(115200);
  delay(250);
  Wire.begin(kI2cSdaPin, kI2cSclPin);

  Serial.printf("I2C configured SDA=%d SCL=%d\n", kI2cSdaPin, kI2cSclPin);
  imuReady = imu.begin();

  if (!imuReady) {
    Serial.println("MPU6050 initialization failed, publishing zero payload until sensor is detected");
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
  Serial.println("Publishing angle data...");
  lastPublishAt = now;

  if (!imuReady && (now - lastImuRetryAt >= kImuRetryIntervalMs)) {
    lastImuRetryAt = now;
    imuReady = imu.begin();
    Serial.println(imuReady ? "MPU6050 connected" : "MPU6050 retry failed");
  }

  Mpu6050::Angles angles{0.0f, 0.0f, 0.0f};
  if (imuReady) {
    angles = imu.readAngles();
  }

  const String payload = formatAngles(angles);

  if (rotationCharacteristic != nullptr) {
    rotationCharacteristic->setValue(payload.c_str());
    if (bleClientConnected) {
      rotationCharacteristic->notify();
    }
  }
  NimBLEDevice::getAdvertising()->setName(kDeviceName);

  Serial.println(payload);
}