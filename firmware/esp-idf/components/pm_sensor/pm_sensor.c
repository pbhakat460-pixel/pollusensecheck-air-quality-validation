/******************************************************************************
 * File Name   : pm_sensor.c
 * Project     : PolluSense
 * Description :
 * Implements the I2C driver for the PM sensor used in the PolluSense system.
 *
 * This module is responsible for:
 *   - Initializing the PM sensor.
 *   - Reading sensor registers.
 *   - Writing sensor registers.
 *   - Controlling sensor sleep and wake modes.
 *   - Reading firmware version.
 *   - Reading 16-bit sensor register values.
 ******************************************************************************/

#include "pm_sensor.h"          /* PM sensor interface */
#include "driver/i2c.h"         /* ESP-IDF I2C driver */
#include "freertos/FreeRTOS.h"  /* FreeRTOS core definitions */
#include "freertos/task.h"      /* FreeRTOS task and delay functions */
#include "esp_log.h"            /* ESP-IDF logging utilities */
#include "pins.h"               /* Hardware pin definitions */
#include "i2c_bus.h"            /* Dynamic I2C bus manager */

/* Logging tag for the PM sensor driver */
static const char *TAG = "PM_SENSOR";

/*------------------------------------------------------------*/
/******************************************************************************
 * Function    : pm_sensor_init
 *
 * Description :
 * Initializes the PM sensor by selecting its I2C communication bus.
 *
 * Parameters  : None
 *
 * Returns
 *      ESP_OK on successful initialization.
 ******************************************************************************/
esp_err_t pm_sensor_init(void)
{
    /* Select the I2C bus connected to the PM sensor */
    ESP_ERROR_CHECK(i2c_bus_select(I2C_BUS_PM));

    /* Indicate that the sensor is ready for communication */
    ESP_LOGI(TAG, "PM Sensor Ready");

    return ESP_OK;
}

/*------------------------------------------------------------*/
/******************************************************************************
 * Function    : write_reg
 *
 * Description :
 * Writes a single byte to a specified PM sensor register.
 *
 * Parameters
 *      reg   : Register address.
 *      value : Data byte to be written.
 *
 * Returns
 *      ESP-IDF status code.
 ******************************************************************************/
static esp_err_t write_reg(uint8_t reg, uint8_t value)
{
    /* Register address and value to be transmitted */
    uint8_t data[2] =
    {
        reg,
        value
    };

    /* Select the PM sensor I2C bus */
    ESP_ERROR_CHECK(i2c_bus_select(I2C_BUS_PM));

    /* Write the register and data to the PM sensor */
    return i2c_master_write_to_device(
        i2c_bus_port(),
        PM_I2C_ADDRESS,
        data,
        sizeof(data),
        pdMS_TO_TICKS(100));
}

/*------------------------------------------------------------*/
/******************************************************************************
 * Function    : read_reg
 *
 * Description :
 * Reads one or more bytes from a specified PM sensor register.
 *
 * Parameters
 *      reg    : Register address.
 *      buffer : Buffer to store received data.
 *      len    : Number of bytes to read.
 *
 * Returns
 *      ESP-IDF status code.
 ******************************************************************************/
static esp_err_t read_reg(uint8_t reg, uint8_t *buffer, size_t len)
{
    /* Select the PM sensor I2C bus */
    ESP_ERROR_CHECK(i2c_bus_select(I2C_BUS_PM));

    /* Read data from the requested register */
    return i2c_master_write_read_device(
        i2c_bus_port(),
        PM_I2C_ADDRESS,
        &reg,
        1,
        buffer,
        len,
        pdMS_TO_TICKS(100));
}

/*------------------------------------------------------------*/
/******************************************************************************
 * Function    : pm_sensor_awake
 *
 * Description :
 * Wakes the PM sensor from low-power sleep mode.
 *
 * Parameters  : None
 *
 * Returns
 *      ESP-IDF status code.
 ******************************************************************************/
esp_err_t pm_sensor_awake(void)
{
    /* Log sensor wake-up operation */
    ESP_LOGI(TAG, "Sensor Awake");

    /* Write wake command to the control register */
    return write_reg(0x01, 0x02);
}

/*------------------------------------------------------------*/
/******************************************************************************
 * Function    : pm_sensor_sleep
 *
 * Description :
 * Places the PM sensor into low-power sleep mode.
 *
 * Parameters  : None
 *
 * Returns
 *      ESP-IDF status code.
 ******************************************************************************/
esp_err_t pm_sensor_sleep(void)
{
    /* Log sensor sleep operation */
    ESP_LOGI(TAG, "Sensor Sleep");

    /* Write sleep command to the control register */
    return write_reg(0x01, 0x01);
}

/*------------------------------------------------------------*/
/******************************************************************************
 * Function    : pm_sensor_read_version
 *
 * Description :
 * Reads the firmware version stored inside the PM sensor.
 *
 * Parameters
 *      version : Pointer to store the firmware version.
 *
 * Returns
 *      ESP-IDF status code.
 ******************************************************************************/
esp_err_t pm_sensor_read_version(uint8_t *version)
{
    /* Read firmware version register */
    esp_err_t ret = read_reg(0x1D, version, 1);

    /* Display the firmware version if the read succeeds */
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Firmware Version = %u", *version);
    }
    else
    {
        /* Report communication failure */
        ESP_LOGE(TAG, "Read Version Failed (%s)", esp_err_to_name(ret));
    }

    return ret;
}

/******************************************************************************
 * Function    : pm_sensor_read_u16
 *
 * Description :
 * Reads a 16-bit value from the specified PM sensor register.
 *
 * Parameters
 *      reg   : Register address.
 *      value : Pointer to store the received 16-bit value.
 *
 * Returns
 *      ESP_OK on success, otherwise an ESP-IDF error code.
 ******************************************************************************/
esp_err_t pm_sensor_read_u16(uint8_t reg, uint16_t *value)
{
    /* Temporary buffer for the received bytes */
    uint8_t buf[2];

    /* Read two bytes from the requested register */
    esp_err_t ret = read_reg(reg, buf, 2);

    /* Stop if the register read fails */
    if (ret != ESP_OK)
    {
        return ret;
    }

    /* Combine the received bytes into a 16-bit value */
    *value = ((uint16_t)buf[0] << 8) | buf[1];

    return ESP_OK;
}