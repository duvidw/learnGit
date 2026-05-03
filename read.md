# ESP32 NimBLE MPU6050 Server

This project runs an ESP32 BLE server that reads MPU6050 orientation data and publishes it as a BLE characteristic payload.

## Features

- NimBLE server (NimBLE-Arduino 2.5.0)
- MPU6050 via Adafruit MPU6050 library
- BLE notify + read characteristic
- Published data:
  - pitch (degrees)
  - roll (degrees)
  - yaw (degrees, gyro-integrated, relative and drifting over time)

## Project Environments

Defined in `platformio.ini`:

- `esp32s3_120_16_8-qio_opi`
- `esp32-c3-supermini`

## Libraries

- `h2zero/NimBLE-Arduino@2.5.0`
- `adafruit/Adafruit MPU6050@^2.2.6`
- `adafruit/Adafruit Unified Sensor@^1.1.14` (C3 env)

## Wiring (ESP32-C3 Super Mini)

Current code uses:

- SDA: GPIO 5
- SCL: GPIO 6
- MPU6050 address: `0x68`

Also connect power and ground:

- VCC -> 3.3V
- GND -> GND

## BLE Details

- Device name: `ESP32-MPU6050`
- Service UUID: `6df0f54f-5a5f-4f53-a31d-2160c7d2f001`
- Characteristic UUID: `6df0f54f-5a5f-4f53-a31d-2160c7d2f002`
- Properties: READ, NOTIFY
- Payload format:

`pitch=xx.xx,roll=yy.yy,yaw=zz.zz`

Publish interval is 500 ms.

## Build / Upload

From project root:

```powershell
pio run -e esp32-c3-supermini
pio run -e esp32-c3-supermini -t upload --upload-port COM24
```

If upload fails with COM port busy, close the serial monitor first.

## Serial Output

The firmware prints setup and payload updates to serial at `115200` baud.

## Notes on Yaw

Yaw is computed by integrating gyro Z rate over time:

- good for short-term relative rotation
- will drift over time
- not true absolute heading without a magnetometer

For true absolute heading, use a 9-axis sensor fusion setup (gyro + accel + magnetometer).
