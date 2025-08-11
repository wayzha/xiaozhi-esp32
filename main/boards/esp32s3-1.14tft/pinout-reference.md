# ESP32S3 1.14TFT 引脚映射参考

## 概述

本文档提供ESP32S3 1.14TFT开发板的引脚映射参考。由于不同厂商的硬件设计可能有所差异，请根据实际的PCB原理图调整 `config.h` 中的引脚定义。

## ESP32S3 引脚功能说明

### GPIO功能分类

| 引脚范围 | 功能说明 | 推荐用途 |
|---------|---------|---------|
| GPIO0 | 启动模式控制 | Boot按钮 |
| GPIO1-2 | 通用IO | LED、ADC |
| GPIO3-8 | 通用IO | I2C、SPI、控制信号 |
| GPIO9-14 | 通用IO | I2S音频、SPI |
| GPIO15-16 | 通用IO | I2S音频 |
| GPIO17-18 | 通用IO | SPI显示屏 |
| GPIO19-20 | USB D-/D+ | USB通信 |
| GPIO21 | 通用IO | 功放控制 |
| GPIO22-23 | 通用IO | SPI显示屏 |
| GPIO26-48 | 通用IO | 扩展功能 |

## 当前配置映射

### 音频系统引脚
```c
// I2S音频接口
#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_16  // 主时钟输出
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_15  // 字选择信号
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_14  // 位时钟
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_13  // 数据输入（麦克风）
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_12  // 数据输出（扬声器）

// 音频编解码器控制
#define AUDIO_CODEC_PA_PIN       GPIO_NUM_21  // 功放使能
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_8   // I2C数据线
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_9   // I2C时钟线
```

### 显示屏系统引脚
```c
// SPI显示屏接口
#define DISPLAY_SPI_SCK_PIN  GPIO_NUM_18  // SPI时钟
#define DISPLAY_SPI_MOSI_PIN GPIO_NUM_23  // SPI数据输出
#define DISPLAY_DC_PIN       GPIO_NUM_4   // 数据/命令选择
#define DISPLAY_SPI_CS_PIN   GPIO_NUM_5   // 片选信号
#define DISPLAY_RST_PIN      GPIO_NUM_17  // 复位信号
#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_22 // 背光控制
```

### 控制和状态引脚
```c
// 用户交互
#define BOOT_BUTTON_GPIO    GPIO_NUM_0   // 启动按钮
#define BUILTIN_LED_GPIO    GPIO_NUM_2   // 状态LED

// 电源管理
#define BATTERY_ADC_PIN     GPIO_NUM_1   // 电池电压检测
#define CHARGING_STATUS_PIN GPIO_NUM_3   // 充电状态检测
```

## 替代引脚方案

### 方案A：紧凑型布局
适用于引脚资源紧张的设计：

```c
// 音频系统（使用连续引脚）
#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_10
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_11
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_12
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_13
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_14

// 显示屏系统
#define DISPLAY_SPI_SCK_PIN  GPIO_NUM_6
#define DISPLAY_SPI_MOSI_PIN GPIO_NUM_7
#define DISPLAY_DC_PIN       GPIO_NUM_4
#define DISPLAY_SPI_CS_PIN   GPIO_NUM_5
#define DISPLAY_RST_PIN      GPIO_NUM_15
```

### 方案B：分离式布局
适用于需要隔离干扰的设计：

```c
// 音频系统（远离数字信号）
#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_35
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_36
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_37
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_38
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_39

// 显示屏系统（使用高速IO）
#define DISPLAY_SPI_SCK_PIN  GPIO_NUM_18
#define DISPLAY_SPI_MOSI_PIN GPIO_NUM_23
#define DISPLAY_DC_PIN       GPIO_NUM_4
#define DISPLAY_SPI_CS_PIN   GPIO_NUM_5
```

## 引脚约束和注意事项

### 1. 启动相关引脚
```c
// 这些引脚在启动时有特殊功能，使用时需要注意
GPIO0  - 启动模式选择（建议用作Boot按钮）
GPIO3  - JTAG功能，可用作普通IO
GPIO45 - VDD_SPI，需要特殊配置
GPIO46 - 启动时为输入模式
```

### 2. USB功能引脚
```c
// ESP32S3内置USB功能，这两个引脚通常被占用
GPIO19 - USB D-
GPIO20 - USB D+
```

### 3. PSRAM相关引脚
```c
// 如果使用外部PSRAM，以下引脚被占用
GPIO26-32 - PSRAM接口（QSPI模式）
```

### 4. ADC功能引脚
```c
// 可用于模拟信号采集的引脚
GPIO1-10   - ADC1通道
GPIO11-20  - ADC2通道（注意：WiFi使用时ADC2不可用）
```

## 硬件设计建议

### 1. 信号完整性
- 高速信号（SPI时钟）使用短走线
- 音频信号远离数字开关信号
- 为敏感信号添加地线保护

### 2. 电源设计
- 为音频部分提供独立的电源滤波
- 显示屏背光使用PWM控制，添加适当的滤波电容

### 3. 布局建议
```
推荐布局：
[USB] [ESP32S3] [显示屏]
[电池] [音频芯片] [扬声器]
```

## 调试引脚配置

### 串口调试
```c
// ESP32S3内置USB串口，无需额外引脚
// 也可以使用传统UART
#define DEBUG_UART_TX GPIO_NUM_43
#define DEBUG_UART_RX GPIO_NUM_44
```

### JTAG调试
```c
// 如果需要JTAG调试功能
#define JTAG_TMS GPIO_NUM_42
#define JTAG_TDI GPIO_NUM_41
#define JTAG_TCK GPIO_NUM_40
#define JTAG_TDO GPIO_NUM_39
```

## 配置验证清单

在修改引脚配置后，请检查以下项目：

- [ ] 所有引脚都在有效范围内（GPIO0-48）
- [ ] 没有引脚冲突（一个引脚只能有一个功能）
- [ ] 启动相关引脚配置正确
- [ ] USB引脚未被误用
- [ ] ADC引脚配置合理
- [ ] 高速信号引脚选择适当
- [ ] 电源管理引脚配置正确

## 测试建议

### 1. 引脚功能测试
```c
// 在初始化代码中添加引脚测试
void test_gpio_config() {
    // 测试输出引脚
    gpio_set_level(BUILTIN_LED_GPIO, 1);
    vTaskDelay(500);
    gpio_set_level(BUILTIN_LED_GPIO, 0);
    
    // 测试输入引脚
    int button_state = gpio_get_level(BOOT_BUTTON_GPIO);
    ESP_LOGI("TEST", "Button state: %d", button_state);
}
```

### 2. 通信接口测试
- I2C扫描：检测音频编解码器地址
- SPI测试：发送显示屏初始化命令
- ADC测试：读取电池电压

通过系统性的引脚配置和测试，可以确保ESP32S3 1.14TFT开发板的硬件功能正常工作。