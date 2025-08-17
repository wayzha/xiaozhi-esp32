#include "wifi_board.h"
#include "codecs/dummy_audio_codec.h"  // No audio codec on this board
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "i2c_device.h"
#include "led/single_led.h"
#include "led/gpio_led.h"

#include <esp_log.h>
#include <esp_lcd_panel_vendor.h>
#include <driver/i2c_master.h>
#include <driver/spi_common.h>
#include <wifi_station.h>

#define TAG "ESP32S3114TFT"

LV_FONT_DECLARE(font_puhui_16_4);
LV_FONT_DECLARE(font_awesome_16_4);

class Esp32S3114TFT : public WifiBoard {
private:
    i2c_master_bus_handle_t sensor_i2c_bus_;  // For BMP280 and QMI8658C sensors
    Button boot_button_;
    LcdDisplay* display_;
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;

    void InitializeI2c() {
        // Initialize I2C for sensors (BMP280, QMI8658C)
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = SENSOR_I2C_SDA_PIN,
            .scl_io_num = SENSOR_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        
        if (SENSOR_I2C_SDA_PIN != GPIO_NUM_NC && SENSOR_I2C_SCL_PIN != GPIO_NUM_NC) {
            ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &sensor_i2c_bus_));
            ESP_LOGI(TAG, "I2C bus initialized for sensors");
        } else {
            ESP_LOGW(TAG, "I2C pins not configured - sensors will not be available");
        }
    }

    void InitializeTftPower() {
        // Enable TFT power as mentioned in specs
        if (TFT_I2C_POWER_PIN != GPIO_NUM_NC) {
            gpio_config_t io_conf = {
                .pin_bit_mask = (1ULL << TFT_I2C_POWER_PIN),
                .mode = GPIO_MODE_OUTPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_DISABLE,
            };
            gpio_config(&io_conf);
            gpio_set_level(TFT_I2C_POWER_PIN, 1);  // Pull high to enable TFT
            ESP_LOGI(TAG, "TFT power enabled");
        }
    }

    void InitializeSpi() {
        if (DISPLAY_SPI_SCK_PIN == GPIO_NUM_NC || DISPLAY_SPI_MOSI_PIN == GPIO_NUM_NC) {
            ESP_LOGW(TAG, "Display SPI pins not configured - display will not work");
            return;
        }

        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = DISPLAY_SPI_MOSI_PIN;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = DISPLAY_SPI_SCK_PIN;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }
            app.ToggleChatState();
        });
    }

    void InitializeSt7789Display() {
        if (DISPLAY_DC_PIN == GPIO_NUM_NC || DISPLAY_SPI_CS_PIN == GPIO_NUM_NC) {
            ESP_LOGW(TAG, "Display control pins not configured - display will not work");
            return;
        }

        // LCD panel IO initialization
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

        // Initialize ST7789 LCD driver
        ESP_LOGD(TAG, "Install LCD driver");
        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = DISPLAY_RST_PIN;  // May be GPIO_NUM_NC
        panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
        panel_config.bits_per_pixel = 16;
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io_, &panel_config, &panel_));
        
        esp_lcd_panel_reset(panel_);
        esp_lcd_panel_init(panel_);
        esp_lcd_panel_invert_color(panel_, true);
        esp_lcd_panel_swap_xy(panel_, DISPLAY_SWAP_XY);
        esp_lcd_panel_mirror(panel_, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
        
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
    Esp32S3114TFT() : boot_button_(BOOT_BUTTON_GPIO) {
        InitializeTftPower();  // Must be done first
        InitializeI2c();
        InitializeSpi();
        InitializeSt7789Display();
        InitializeButtons();
        
        if (GetBacklight()) {
            GetBacklight()->SetBrightness(80);  // Set reasonable default brightness
        }
        
        ESP_LOGI(TAG, "ESP32-S3 1.14TFT board initialized");
    }

    virtual AudioCodec* GetAudioCodec() override {
        // This board doesn't have a dedicated audio codec
        // Using dummy codec - you could implement ADC microphone here if needed
        static DummyAudioCodec audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE);
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        return display_;
    }
    
    virtual Backlight* GetBacklight() override {
        if (DISPLAY_BACKLIGHT_PIN != GPIO_NUM_NC) {
            static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
            return &backlight;
        }
        return nullptr;
    }

    virtual Led* GetLed() override {
        if (RGB_LED_GPIO != GPIO_NUM_NC) {
            // RGB NeoPixel LED
            static SingleLed led(RGB_LED_GPIO);
            return &led;
        } else if (BUILTIN_LED_GPIO != GPIO_NUM_NC) {
            // Red LED fallback
            static GpioLed led(BUILTIN_LED_GPIO);
            return &led;
        }
        return nullptr;
    }

    virtual bool GetTemperature(float& temperature) override {
        // Could implement BMP280 temperature reading here
        // For now, return false to indicate not implemented
        return false;
    }
};

DECLARE_BOARD(Esp32S3114TFT);