# ESP32-S3 1.14TFT Board

## Overview

ESP32-S3 1.14TFT is a development board based on the ESP32-S3FH4R2 chip featuring a 1.14-inch TFT display. It includes a PH2.0 2P power connector for 3.7V lithium battery and USB charging capability.

**ESP32-S3FH4R2 Specifications:**
- **Flash Memory**: 4MB (F = embedded Flash)
- **PSRAM**: 2MB (H = embedded PSRAM, QUAD mode)
- **CPU**: Dual-core Xtensa LX7 up to 240MHz with AI/ML instruction set
- **WiFi**: 2.4GHz 802.11 b/g/n

## Hardware Features

### Display
- **Screen**: 1.14-inch IPS TFT
- **Resolution**: 240×135 pixels
- **Driver**: ST7789 chipset
- **Colors**: Full color with wide viewing angle

### Sensors
- **BMP280**: Temperature and pressure sensor (I2C address 0x6B)
  - Pressure measurement accuracy: ±1 hPa
  - Temperature measurement accuracy: ±1.0°C
  - Can be used as altimeter with ±1m accuracy
- **QMI8658C**: 6-axis gyroscope and accelerometer (I2C address 0x77)

### LEDs
- **RGB LED**: NeoPixel on GPIO33
- **Red LED**: Status LED on GPIO13

### Power Management
- **USB Type-C**: Power supply and battery charging
- **PH2.0 Connector**: For 3.7V lithium battery (250mAh or larger)
- **CHG LED**: Green charging indicator
- **Battery Voltage**: Available on BAT pin
- **3.3V Output**: 500mA peak current capability

### Connectivity
- **WiFi**: ESP32-S3 built-in 2.4GHz WiFi
- **I2C**: STEMMA QT connector for sensor expansion
- **SH1.0 4P**: Qwiic compatible connector

### Buttons
- **RST Button**: Reset and bootloader entry
- **BOOT Button**: User input and ROM bootloader mode (GPIO0)

## Pin Configuration

**Note**: Some pin assignments need to be confirmed from the actual hardware pinout diagram.

### Display (ST7789)
- **Resolution**: 240×135 pixels
- **SPI Interface**: Needs pin confirmation
- **Power Control**: TFT_I2C_POWER pin (must be high)

### I2C (Sensors)
- **SCL/SDA**: Shared by STEMMA QT connector and onboard sensors

### GPIO Pins
- **A0-A5**: Analog input pins
- **D5-D6, D9-D13**: Digital I/O pins
- **High-speed SPI**: SCK, MOSI, MISO pins available

## Current Implementation Status

### ✅ Implemented
- Basic board structure
- WiFi connectivity (inherited from WifiBoard)
- Display framework (ST7789 driver)
- Button handling (BOOT button)
- Power management framework
- LED control (RGB and red LED)

### ⚠️ Needs Pin Configuration
The following pins need to be confirmed from the hardware pinout:
- Display SPI pins (SCK, MOSI, CS, DC)
- TFT power control pin
- Display backlight control pin
- I2C pins for sensors (SCL, SDA)

### 🚧 TODO
- Complete pin assignment in config.h
- Implement BMP280 temperature/pressure reading
- Implement QMI8658C motion sensing
- Add battery monitoring
- Audio support (if external microphone/speaker added)

## Building and Flashing

1. **Configure pins**: Update `config.h` with correct pin assignments
2. **Build**: `python scripts/release.py esp32s3-1.14tft`
3. **Flash**: Use esptool or ESP-IDF flash tools

## Pin Assignment Help Needed

To complete this board implementation, the following pin assignments from the hardware pinout are needed:

1. **TFT Display SPI**:
   - SCK (Serial Clock)
   - MOSI (Master Out Slave In)
   - CS (Chip Select)
   - DC (Data/Command)
   - Reset (if connected)

2. **TFT Control**:
   - TFT_I2C_POWER pin
   - Backlight control pin

3. **I2C Sensors**:
   - SDA (Serial Data)
   - SCL (Serial Clock)

## Development Notes

- This board does not include a dedicated audio codec
- Audio functionality would require external I2S microphone/speaker
- The implementation uses a dummy audio codec by default
- All GPIO pins support PWM, I2C, SPI, and UART through ESP32-S3 pin muxing
- The board supports both Arduino and CircuitPython development

## Power Consumption

- Deep sleep modes supported through ESP32-S3
- Battery level monitoring available
- USB charging management included

## References

- [NoLogo Tech ESP32-S3 1.14TFT Product Page](https://www.nologo.tech/product/esp32/esp32s3/esp32S31.14TFT/esp32S31.14TFT.html)
- ESP32-S3 Technical Reference Manual
- ST7789 Display Driver Documentation