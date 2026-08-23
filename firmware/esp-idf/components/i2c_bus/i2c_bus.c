/******************************************************************************
 * File Name   : i2c_bus.c
 * Project     : PolluSense
 * Description :
 * Implements the I2C bus manager for the PolluSense system.
 *
 * This module allows a single ESP32-S3 I2C controller to communicate with
 * multiple peripherals by dynamically reconfiguring the SDA and SCL pins.
 *
 * Supported devices:
 *   - OLED Display
 *   - PMSA7003T Sensor
 *   - K30 CO2 Sensor
 *   - RTC (Reserved)
 ******************************************************************************/
/* I2C bus manager interface */
#include "i2c_bus.h"
/* Hardware GPIO configuration */
#include "pins.h"
/* ESP-IDF logging utilities */
#include "esp_log.h"
/* Logging tag for the I2C bus manager */
static const char *TAG = "I2C_BUS";
/* ESP32 I2C controller used by all peripherals */
#define BUS_PORT I2C_NUM_1
/* Tracks the currently configured I2C device */
static i2c_bus_device_t current_bus = -1;
/******************************************************************************
 * Function    : i2c_bus_port
 *
 * Description :
 * Returns the ESP32 I2C controller used by the application.
 *
 * Parameters  : None
 *
 * Returns
 *      I2C port identifier.
 ******************************************************************************/
i2c_port_t i2c_bus_port(void)
{
    return BUS_PORT;
}
/******************************************************************************
 * Function    : configure_bus
 *
 * Description :
 * Reconfigures the ESP32 I2C controller with the GPIO pins required
 * for the selected peripheral.
 *
 * Parameters
 *      sda : SDA GPIO pin
 *      scl : SCL GPIO pin
 *
 * Returns
 *      ESP_OK on success.
 ******************************************************************************/
static esp_err_t configure_bus(gpio_num_t sda, gpio_num_t scl)
{
    /* Remove the current I2C driver before reconfiguration */
    i2c_driver_delete(BUS_PORT);
    /* Configure I2C controller in master mode */
    i2c_config_t cfg =
    {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000/* Standard I2C clock frequency (100 kHz) */
    };
    /* Apply the new I2C configuration */
    ESP_ERROR_CHECK(i2c_param_config(BUS_PORT, &cfg));
    /* Install the I2C driver using the selected configuration */
    esp_err_t ret = i2c_driver_install(
        BUS_PORT,
        cfg.mode,
        0,
        0,
        0);
    /* Ignore "already installed" state, return all other errors */
    if (ret != ESP_OK &&
        ret != ESP_ERR_INVALID_STATE)
    {
        return ret;
    }

    return ESP_OK;
}
/******************************************************************************
 * Function    : i2c_bus_init
 *
 * Description :
 * Initializes the I2C bus manager and resets the currently
 * selected device.
 *
 * Parameters  : None
 *
 * Returns
 *      ESP_OK.
 ******************************************************************************/
esp_err_t i2c_bus_init(void)
{
    current_bus = -1;/* No device is selected during startup */

    ESP_LOGI(TAG, "I2C Bus Manager Ready");

    return ESP_OK;
}
/******************************************************************************
 * Function    : i2c_bus_select
 *
 * Description :
 * Selects an I2C peripheral by reconfiguring the ESP32 I2C controller
 * with the GPIO pins assigned to that device.
 *
 * Parameters
 *      device : Target I2C peripheral
 *
 * Returns
 *      ESP_OK on success, otherwise an ESP-IDF error code.
 ******************************************************************************/
esp_err_t i2c_bus_select(i2c_bus_device_t device)
{
    /* Skip reconfiguration if the requested device is already active */
    if (device == current_bus)
    {
        return ESP_OK;
    }

    esp_err_t ret;/* Stores the result of I2C bus configuration */

    switch(device)/* Select GPIO configuration based on the requested peripheral */
    {
        case I2C_BUS_OLED:/* Configure bus for OLED display */

            ret = configure_bus(
                    OLED_I2C_SDA,
                    OLED_I2C_SCL);

            break;

        case I2C_BUS_PM:/* Configure bus for PMSA7003T sensor */

            ret = configure_bus(
                    PM_I2C_SDA,
                    PM_I2C_SCL);

            break;

        case I2C_BUS_K30:/* Configure bus for K30 CO2 sensor */

            ret = configure_bus(
                    K30_I2C_SDA,
                    K30_I2C_SCL);

            break;

        case I2C_BUS_RTC:/* Configure bus for Real-Time Clock */

            ret = configure_bus(
                    GPIO_NUM_0,
                    GPIO_NUM_45);

            break;

        default:/* Invalid device selection */

            return ESP_ERR_INVALID_ARG;
    }

    if (ret == ESP_OK)if (ret == ESP_OK)
    {
        current_bus = device;
    }

    return ret;
}