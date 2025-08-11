#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_adc/adc_cali.h>
#include <functional>

/**
 * @brief 电源管理类
 * 
 * 负责管理电池电量检测、充电状态检测等功能
 */
class PowerManager {
public:
    using ChargingStatusCallback = std::function<void(bool is_charging)>;

private:
    gpio_num_t charging_pin_;
    adc_oneshot_unit_handle_t adc_handle_;
    adc_cali_handle_t adc_cali_handle_;
    bool adc_calibrated_;
    ChargingStatusCallback charging_callback_;
    bool last_charging_status_;

public:
    /**
     * @brief 构造函数
     * @param charging_pin 充电状态检测引脚，GPIO_NUM_NC表示不使用
     */
    PowerManager(gpio_num_t charging_pin = GPIO_NUM_NC);

    /**
     * @brief 析构函数
     */
    ~PowerManager();

    /**
     * @brief 获取电池电量百分比
     * @return 电池电量百分比 (0-100)
     */
    int GetBatteryLevel();

    /**
     * @brief 获取电池电压
     * @return 电池电压 (mV)
     */
    int GetBatteryVoltage();

    /**
     * @brief 检查是否正在充电
     * @return true表示正在充电，false表示未充电
     */
    bool IsCharging();

    /**
     * @brief 检查是否正在放电
     * @return true表示正在放电，false表示未放电
     */
    bool IsDischarging();

    /**
     * @brief 设置充电状态变化回调函数
     * @param callback 回调函数
     */
    void OnChargingStatusChanged(ChargingStatusCallback callback);

    /**
     * @brief 更新充电状态（需要定期调用）
     */
    void Update();

private:
    /**
     * @brief 初始化ADC
     */
    void InitializeAdc();

    /**
     * @brief 初始化充电检测引脚
     */
    void InitializeChargingPin();

    /**
     * @brief 读取ADC原始值
     * @return ADC原始值
     */
    int ReadAdcRaw();

    /**
     * @brief 将ADC原始值转换为电压
     * @param raw_value ADC原始值
     * @return 电压值 (mV)
     */
    int ConvertToVoltage(int raw_value);
};

#endif // _POWER_MANAGER_H_