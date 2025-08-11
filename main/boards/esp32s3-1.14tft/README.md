# ESP32S3 1.14寸TFT开发板

## 概述

ESP32S3 1.14寸TFT开发板是一款基于ESP32S3FH4R2芯片的紧凑型开发板，集成了1.14寸TFT彩色显示屏、音频编解码器、锂电池管理等功能，适合用于小智AI语音聊天机器人项目。

## 硬件特性

### 主控芯片
- **芯片型号**: ESP32S3FH4R2
- **CPU**: 双核Xtensa LX7处理器，主频240MHz
- **Flash**: 4MB
- **PSRAM**: 2MB
- **WiFi**: 802.11 b/g/n
- **蓝牙**: Bluetooth 5.0 LE

### 显示屏
- **尺寸**: 1.14寸TFT LCD
- **分辨率**: 240×135像素
- **驱动芯片**: ST7789
- **接口**: SPI
- **颜色深度**: 16位色彩

### 音频系统
- **编解码器**: ES8311
- **采样率**: 输入16kHz，输出24kHz
- **接口**: I2S + I2C控制

### 电源管理
- **供电方式**: USB Type-C + 3.7V锂电池
- **电池接口**: PH2.02P连接器
- **充电功能**: 支持USB充电
- **电池管理**: 内置电源管理电路

### 物理尺寸
- **长度**: 50.80mm
- **宽度**: 25.34mm
- **高度**: 10.8mm（包含显示屏）

## 引脚配置

### 音频引脚
```
I2S_MCLK: GPIO16
I2S_WS:   GPIO15
I2S_BCLK: GPIO14
I2S_DIN:  GPIO13
I2S_DOUT: GPIO12
PA_PIN:   GPIO21
I2C_SDA:  GPIO8
I2C_SCL:  GPIO9
```

### 显示屏引脚
```
SPI_SCK:  GPIO18
SPI_MOSI: GPIO23
DC:       GPIO4
CS:       GPIO5
RST:      GPIO17
BL:       GPIO22
```

### 控制引脚
```
BOOT_BTN: GPIO0
LED:      GPIO2
BAT_ADC:  GPIO1
CHG_STA:  GPIO3
```

## 编译和烧录

### 环境要求
- ESP-IDF v5.0+
- Python 3.8+

### 编译步骤

1. 克隆项目代码：
```bash
git clone <项目地址>
cd xiaozhi-esp32
```

2. 设置ESP-IDF环境：
```bash
. $HOME/esp/esp-idf/export.sh
```

3. 编译固件：
```bash
python scripts/release.py esp32s3-1.14tft
```

4. 烧录固件：
```bash
esptool.py --chip esp32s3 --port /dev/ttyUSB0 --baud 921600 write_flash -z 0x0 build/esp32s3-1.14tft.bin
```

## 使用说明

### 首次配置
1. 上电后，设备会进入配网模式
2. 使用手机连接设备热点进行WiFi配置
3. 配置完成后设备会自动连接网络并激活

### 按钮操作
- **短按Boot按钮**: 开始/停止语音对话
- **长按Boot按钮**: 重置WiFi配置（仅在未连网状态下）

### LED指示
- **常亮**: 设备正常工作
- **闪烁**: 网络连接中或语音识别中
- **熄灭**: 设备休眠或故障

### 电池管理
- 支持3.7V锂电池供电
- USB充电时会自动切换到USB供电
- 低电量时会显示电量警告
- 支持自动省电模式

## 开发说明

### 自定义配置
如需修改引脚配置，请编辑 `config.h` 文件中的相关定义。

### 添加新功能
1. 在 `esp32s3-1.14tft.cc` 中添加初始化代码
2. 重写相应的虚函数
3. 更新 `config.json` 中的编译配置

### 调试
- 使用USB串口进行调试输出
- 波特率：115200
- 支持ESP-IDF监控工具

## 故障排除

### 常见问题

1. **显示屏不亮**
   - 检查SPI连接
   - 确认背光引脚配置
   - 检查电源供应

2. **音频无输出**
   - 检查I2S配置
   - 确认ES8311初始化
   - 检查PA使能引脚

3. **无法连接WiFi**
   - 重置WiFi配置
   - 检查网络环境
   - 确认天线连接

4. **电池不充电**
   - 检查USB连接
   - 确认充电电路
   - 检查电池连接

### 技术支持
如遇到问题，请提供以下信息：
- 硬件版本
- 固件版本
- 错误日志
- 复现步骤

## 许可证
本项目遵循项目根目录下的LICENSE文件。