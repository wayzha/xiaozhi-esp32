# ESP32S3 1.14TFT 开发板编译指南

## 代码文件说明

本开发板包含以下核心文件：

### 1. 配置文件
- **config.h**: 硬件引脚配置和参数定义
- **config.json**: 编译配置和目标芯片设置

### 2. 实现文件
- **esp32s3-1.14tft.cc**: 完整版实现（包含电池管理等高级功能）
- **esp32s3-1.14tft-simple.cc**: 简化版实现（基础功能，便于调试）

### 3. 辅助文件
- **power_manager.h**: 电源管理类定义（可选）
- **README.md**: 详细使用说明
- **build.md**: 本编译指南

## 编译前准备

### 1. 环境要求
```bash
# ESP-IDF 版本
ESP-IDF v5.0 或更高版本

# Python 版本
Python 3.8 或更高版本
```

### 2. 引脚配置检查
在编译前，请根据实际硬件原理图检查并修改 `config.h` 中的引脚定义：

```c
// 重要：以下引脚需要根据实际硬件调整
#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_16  // 音频主时钟
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_15  // 音频字选择
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_14  // 音频位时钟
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_13  // 音频数据输入
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_12  // 音频数据输出

#define DISPLAY_SPI_SCK_PIN  GPIO_NUM_18 // 显示屏时钟
#define DISPLAY_SPI_MOSI_PIN GPIO_NUM_23 // 显示屏数据
#define DISPLAY_DC_PIN       GPIO_NUM_4  // 显示屏数据/命令
#define DISPLAY_SPI_CS_PIN   GPIO_NUM_5  // 显示屏片选
#define DISPLAY_RST_PIN      GPIO_NUM_17 // 显示屏复位
```

## 编译步骤

### 方法一：使用项目编译脚本（推荐）

```bash
# 1. 进入项目根目录
cd xiaozhi-esp32

# 2. 设置ESP-IDF环境
. $HOME/esp/esp-idf/export.sh

# 3. 编译ESP32S3 1.14TFT开发板固件
python scripts/release.py esp32s3-1.14tft
```

### 方法二：手动编译

```bash
# 1. 设置目标芯片
idf.py set-target esp32s3

# 2. 配置项目
idf.py menuconfig

# 3. 编译
idf.py build

# 4. 烧录
idf.py -p /dev/ttyUSB0 flash monitor
```

## 调试建议

### 1. 首次调试
建议先使用简化版实现 (`esp32s3-1.14tft-simple.cc`) 进行调试：

```bash
# 重命名文件进行测试
mv esp32s3-1.14tft.cc esp32s3-1.14tft-full.cc
mv esp32s3-1.14tft-simple.cc esp32s3-1.14tft.cc
```

### 2. 分步调试
1. **显示屏测试**: 先确保显示屏能正常显示
2. **音频测试**: 再测试音频输入输出功能
3. **网络测试**: 最后测试WiFi连接功能

### 3. 常见问题排查

#### 显示屏问题
```c
// 检查这些参数是否正确
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  135
#define DISPLAY_OFFSET_X  40
#define DISPLAY_OFFSET_Y  53
```

#### 音频问题
```c
// 确认音频采样率设置
#define AUDIO_INPUT_SAMPLE_RATE  16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000
```

## 固件烧录

### 1. 连接硬件
- 使用USB-C线连接开发板到电脑
- 确保驱动程序已正确安装

### 2. 烧录命令
```bash
# 自动检测端口烧录
idf.py flash

# 指定端口烧录
idf.py -p /dev/ttyUSB0 flash

# Windows系统
idf.py -p COM3 flash
```

### 3. 监控输出
```bash
# 查看串口输出
idf.py monitor

# 指定端口监控
idf.py -p /dev/ttyUSB0 monitor
```

## 性能优化

### 1. 编译优化
在 `config.json` 中可以添加优化选项：

```json
{
    "target": "esp32s3",
    "builds": [
        {
            "name": "esp32s3-1.14tft",
            "sdkconfig_append": [
                "CONFIG_COMPILER_OPTIMIZATION_SIZE=y",
                "CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE=y"
            ]
        }
    ]
}
```

### 2. 内存优化
```json
"CONFIG_ESP32S3_SPIRAM_SUPPORT=y",
"CONFIG_SPIRAM_MODE_QUAD=y",
"CONFIG_SPIRAM_TYPE_AUTO=y"
```

## 版本管理

建议在开发过程中保留不同版本：

```
esp32s3-1.14tft/
├── esp32s3-1.14tft.cc          # 当前工作版本
├── esp32s3-1.14tft-simple.cc   # 简化版本
├── esp32s3-1.14tft-full.cc     # 完整版本
└── esp32s3-1.14tft-debug.cc    # 调试版本
```

## 技术支持

如果遇到编译或运行问题：

1. 检查ESP-IDF版本兼容性
2. 确认硬件连接正确
3. 查看串口输出日志
4. 参考相似开发板的实现

## 下一步开发

1. **硬件适配**: 根据实际PCB调整引脚配置
2. **功能测试**: 逐一验证各个功能模块
3. **性能调优**: 优化显示刷新率和音频质量
4. **电源管理**: 实现完整的电池管理功能