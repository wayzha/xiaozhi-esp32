#include "wifi_board.h"
#include "codecs/es8311_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "led/single_led.h"

#include <esp_log.h>
#include <esp_lcd_panel_vendor.h>
#include <driver/i2c_master.h>
#include <driver/spi_common.h>
#include <wifi_station.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>

#define TAG "ESP32S3_1_14TFT"

// 声明字体
LV_FONT_DECLARE(font_puhui_16_4);
LV_FONT_DECLARE(font_awesome_16_4);

class Esp32s3114TFT : public WifiBoard {
private:
    i2c_master_bus_handle_t codec_i2c_bus_;
    Button boot_button_;
    LcdDisplay* display_;
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;
    
    // ADC相关
    adc_oneshot_unit_handle_t adc1_handle_;
    adc_cali_handle_t adc1_cali_handle_;
    bool adc_calibrated_;

    void InitializeI2c() {
        // 初始化I2C外设用于音频编解码器
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = AUDIO_CODEC_I2C_SDA_PIN,
            .scl_io_num = AUDIO_CODEC_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &codec_i2c_bus_));
        ESP_LOGI(TAG, "I2C initialized for audio codec");
    }

    void InitializeAdc() {
        // 初始化ADC用于电池电压检测
        adc_oneshot_unit_init_cfg_t init_config1 = {
            .unit_id = ADC_UNIT_1,
        };
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle_));

        adc_oneshot_chan_cfg_t config = {
            .bitwidth = ADC_BITWIDTH_DEFAULT,
            .atten = ADC_ATTEN_DB_11,
        };
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle_, ADC_CHANNEL_0, &config));

        // ADC校准
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = ADC_UNIT_1,
            .atten = ADC_ATTEN_DB_11,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        esp_err_t ret = adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_handle_);
        if (ret == ESP_OK) {
            adc_calibrated_ = true;
            ESP_LOGI(TAG, "ADC calibration initialized");
        } else {
            adc_calibrated_ = false;
            ESP_LOGW(TAG, "ADC calibration failed, using raw values");
        }
    }

    void InitializeSpi() {
        // 初始化SPI总线用于显示屏
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = DISPLAY_SPI_MOSI_PIN;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = DISPLAY_SPI_SCK_PIN;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
        ESP_LOGI(TAG, "SPI initialized for display");
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }
            app.ToggleChatState();
        });
        ESP_LOGI(TAG, "Buttons initialized");
    }

    void InitializeSt7789Display() {
        // 配置复位引脚
        gpio_config_t rst_config = {
            .pin_bit_mask = (1ULL << DISPLAY_RST_PIN),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&rst_config));
        
        // 复位显示屏
        gpio_set_level(DISPLAY_RST_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(20));
        gpio_set_level(DISPLAY_RST_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(20));

        // 液晶屏控制IO初始化
        ESP_LOGD(TAG, "Install panel IO");
        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.cs_gpio_num = DISPLAY_SPI_CS_PIN;
        io_config.dc_gpio_num = DISPLAY_DC_PIN;
        io_config.spi_mode = 0;
        io_config.pclk_hz = 80 * 1000 * 1000;
        io_config.trans_queue_depth = 10;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &panel_io_));

        // 初始化液晶屏驱动芯片ST7789
        ESP_LOGD(TAG, "Install LCD driver");
        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = DISPLAY_RST_PIN;
        panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
        panel_config.bits_per_pixel = 16;
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io_, &panel_config, &panel_));
        
        // 配置显示屏
        ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_));
        ESP_ERROR_CHECK(esp_lcd_panel_init(panel_));
        ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_, true));
        ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_, DISPLAY_SWAP_XY));
        ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y));
        
        // 创建显示屏对象
        display_ = new SpiLcdDisplay(panel_io_, panel_,
                                    DISPLAY_WIDTH, DISPLAY_HEIGHT, 
                                    DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, 
                                    DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY,
                                    {
                                        .text_font = &font_puhui_16_4,
                                        .icon_font = &font_awesome_16_4,
                                        .emoji_font = font_emoji_32_init(),
                                    });
        ESP_LOGI(TAG, "ST7789 display initialized");
    }

public:
    Esp32s3114TFT() : boot_button_(BOOT_BUTTON_GPIO) {
        ESP_LOGI(TAG, "Initializing ESP32S3 1.14TFT Board");
        
        InitializeI2c();
        InitializeAdc();
        InitializeSpi();
        InitializeSt7789Display();
        InitializeButtons();
        
        // 设置背光亮度
        GetBacklight()->SetBrightness(80);
        
        ESP_LOGI(TAG, "ESP32S3 1.14TFT Board initialization completed");
    }

    virtual Led* GetLed() override {
        static SingleLed led(BUILTIN_LED_GPIO);
        return &led;
    }

    virtual AudioCodec* GetAudioCodec() override {
        static Es8311AudioCodec audio_codec(
            codec_i2c_bus_, 
            I2C_NUM_0, 
            AUDIO_INPUT_SAMPLE_RATE, 
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_MCLK, 
            AUDIO_I2S_GPIO_BCLK, 
            AUDIO_I2S_GPIO_WS, 
            AUDIO_I2S_GPIO_DOUT, 
            AUDIO_I2S_GPIO_DIN,
            AUDIO_CODEC_PA_PIN, 
            AUDIO_CODEC_ES8311_ADDR);
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        return display_;
    }
    
    virtual Backlight* GetBacklight() override {
        static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
        return &backlight;
    }

    virtual bool GetBatteryLevel(int& level, bool& charging, bool& discharging) override {
        // 读取电池电压
        int adc_raw = 0;
        int voltage_mv = 0;
        
        esp_err_t ret = adc_oneshot_read(adc1_handle_, ADC_CHANNEL_0, &adc_raw);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "ADC read failed");
            return false;
        }

        if (adc_calibrated_) {
            ret = adc_cali_raw_to_voltage(adc1_cali_handle_, adc_raw, &voltage_mv);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "ADC calibration failed");
                return false;
            }
        } else {
            // 使用简单的线性转换
            voltage_mv = (adc_raw * 3300) / 4095;
        }

        // 电池电压范围：3.0V-4.2V，考虑分压电路
        // 这里需要根据实际硬件的分压电路调整
        int battery_mv = voltage_mv * 2; // 假设1:1分压
        
        // 计算电池电量百分比
        if (battery_mv >= 4200) {
            level = 100;
        } else if (battery_mv <= 3000) {
            level = 0;
        } else {
            level = (battery_mv - 3000) * 100 / (4200 - 3000);
        }

        // 检测充电状态（需要根据实际硬件实现）
        charging = gpio_get_level(CHARGING_STATUS_PIN) == 0; // 假设低电平表示充电
        discharging = !charging;

        return true;
    }

    virtual void SetPowerSaveMode(bool enabled) override {
        if (enabled) {
            // 进入省电模式
            GetBacklight()->SetBrightness(10);
            GetDisplay()->SetPowerSaveMode(true);
        } else {
            // 退出省电模式
            GetBacklight()->RestoreBrightness();
            GetDisplay()->SetPowerSaveMode(false);
        }
        WifiBoard::SetPowerSaveMode(enabled);
    }

    ~Esp32s3114TFT() {
        if (adc_calibrated_) {
            adc_cali_delete_scheme_curve_fitting(adc1_cali_handle_);
        }
        adc_oneshot_del_unit(adc1_handle_);
    }
};

// 注册开发板
DECLARE_BOARD(Esp32s3114TFT);