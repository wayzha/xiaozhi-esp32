#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// ESP32-S3FH4R2 Chip Configuration:
// - 4MB Flash Memory (F = embedded Flash)
// - 2MB PSRAM (H = embedded PSRAM, QUAD mode)  
// - Dual-core Xtensa LX7 240MHz CPU with AI/ML instructions

// This board appears to not have a dedicated audio codec
// Using ADC microphone configuration for basic audio input
#define AUDIO_INPUT_SAMPLE_RATE  16000
#define AUDIO_OUTPUT_SAMPLE_RATE 16000

// No dedicated I2S audio - using ADC for microphone input
#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_NC
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_NC
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_NC
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_NC
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_NC

// No audio codec on this board
#define AUDIO_CODEC_PA_PIN       GPIO_NUM_NC
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_NC
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_NC

// Button configuration
#define BUILTIN_LED_GPIO        GPIO_NUM_13  // Red LED
#define BOOT_BUTTON_GPIO        GPIO_NUM_0   // BOOT button
#define VOLUME_UP_BUTTON_GPIO   GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC

// RGB LED (NeoPixel)
#define RGB_LED_GPIO            GPIO_NUM_33

// Display SPI configuration - from pinout diagram
#define DISPLAY_SPI_SCK_PIN     GPIO_NUM_36  // FSPICLK
#define DISPLAY_SPI_MOSI_PIN    GPIO_NUM_35  // MOSI  
#define DISPLAY_DC_PIN          GPIO_NUM_39  // TFT_DC
#define DISPLAY_SPI_CS_PIN      GPIO_NUM_7   // TFT_CS
#define DISPLAY_RST_PIN         GPIO_NUM_40  // TFT_RESET

// Display configuration - confirmed from specs
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  135
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY  false

#define DISPLAY_OFFSET_X  40    // Common offset for 240x135 displays
#define DISPLAY_OFFSET_Y  52    // Common offset for 240x135 displays

// TFT power control pin - from pinout diagram
#define TFT_I2C_POWER_PIN       GPIO_NUM_21  // TFT_I2C_POWER

// Backlight configuration - from pinout diagram
#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_45    // TFT_BACKLIGHT
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false

// I2C configuration for sensors - from pinout diagram
#define SENSOR_I2C_SDA_PIN      GPIO_NUM_42  // SDA  
#define SENSOR_I2C_SCL_PIN      GPIO_NUM_41  // SCL

// Sensor I2C addresses
#define BMP280_I2C_ADDR         0x6B  // BMP280 temperature/pressure sensor
#define QMI8658C_I2C_ADDR       0x77  // QMI8658C gyro/accelerometer

#endif // _BOARD_CONFIG_H_