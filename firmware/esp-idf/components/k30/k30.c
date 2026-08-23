/******************************************************************************
 * File Name   : k30.c
 * Project     : PolluSense
 * Description :
 * Implements the driver for the K30 Carbon Dioxide (CO₂) sensor.
 *
 * This module:
 *   - Selects the K30 I2C bus.
 *   - Sends measurement commands.
 *   - Receives sensor responses.
 *   - Verifies checksum integrity.
 *   - Extracts CO₂ concentration in parts per million (ppm).
 ******************************************************************************/
/* K30 sensor interface */
#include "k30.h"
/* Hardware pin definitions */
#include "pins.h"
/* Dynamic I2C bus manager */
#include "i2c_bus.h"
/* ESP-IDF I2C driver */
#include "driver/i2c.h"
/* ESP-IDF logging utilities */
#include "esp_log.h"
/* FreeRTOS timing functions */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/* Logging tag for the K30 driver */
static const char *TAG = "K30";

/* Number of bytes expected in the K30 response packet */
#define K30_RESPONSE_LEN    4


/******************************************************************************
 * Function    : k30_init
 *
 * Description :
 * Initializes the K30 CO₂ sensor by selecting its I2C bus.
 *
 * Parameters  : None
 *
 * Returns
 *      ESP_OK on success, otherwise an ESP-IDF error code.
 ******************************************************************************/
esp_err_t k30_init(void)
{
    esp_err_t ret = i2c_bus_select(I2C_BUS_K30);/* Select the I2C bus connected to the K30 sensor */

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG,
                 "Failed to select K30 I2C bus (%s)",
                 esp_err_to_name(ret));

        return ret;
    }

    ESP_LOGI(TAG, "K30 initialized");

    return ESP_OK;
}
/******************************************************************************
 * Function    : k30_read
 *
 * Description :
 * Reads the current Carbon Dioxide concentration from the K30 sensor.
 *
 * The communication sequence is:
 *   1. Select the K30 I2C bus.
 *   2. Send the measurement command.
 *   3. Wait for the sensor response.
 *   4. Read the response packet.
 *   5. Verify checksum.
 *   6. Extract CO₂ concentration.
 *
 * Parameters
 *      co2_ppm : Pointer to store the measured CO₂ concentration.
 *
 * Returns
 *      ESP_OK on success, otherwise an ESP-IDF error code.
 ******************************************************************************/
esp_err_t k30_read(uint16_t *co2_ppm)
{
    /* Validate output pointer */
    if (co2_ppm == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = i2c_bus_select(I2C_BUS_K30);/* Activate the K30 I2C bus */
    if (ret != ESP_OK)
    {
        return ret;
    }

    /* Wait for the I2C bus to stabilize after switching */
    vTaskDelay(pdMS_TO_TICKS(20));

    static const uint8_t cmd[4] =/* K30 command used to request the latest CO₂ measurement */
    {
        0x22,
        0x00,
        0x08,
        0x2A
    };

    uint8_t rx[4];/* Buffer for the received response packet */

        /*-------------------------------------------------
    * Send the CO₂ measurement request to the sensor.
    *------------------------------------------------*/

    i2c_cmd_handle_t handle = i2c_cmd_link_create();/* Create an I2C command sequence */

    i2c_master_start(handle);/* Start I2C write transaction */
    i2c_master_write_byte(handle,
                          (K30_I2C_ADDRESS << 1) | I2C_MASTER_WRITE,
                          true);/* Send sensor address with write operation */
    i2c_master_write(handle,
                     (uint8_t *)cmd,
                     sizeof(cmd),
                     true);/* Transmit the measurement command */
    i2c_master_stop(handle);/* Finish the write transaction */

    ret = i2c_master_cmd_begin(/* Execute the write transaction */
            i2c_bus_port(),
            handle,
            pdMS_TO_TICKS(1000));

    i2c_cmd_link_delete(handle);/* Release command resources */

    if (ret != ESP_OK)/* Stop if the command transmission failed */
    {
        ESP_LOGE(TAG,
                 "Write failed (%s)",
                 esp_err_to_name(ret));
        return ret;
    }

    /* Allow the sensor sufficient time to prepare its response */
    vTaskDelay(pdMS_TO_TICKS(50));

        /*-------------------------------------------------
    * Read the response packet from the sensor.
    *------------------------------------------------*/

    handle = i2c_cmd_link_create();/* Create a new I2C read transaction */

    i2c_master_start(handle);/* Start I2C read transaction */
    i2c_master_write_byte(handle,
                          (K30_I2C_ADDRESS << 1) | I2C_MASTER_READ,
                          true);/* Send sensor address with read operation */

    i2c_master_read(handle, rx, 3, I2C_MASTER_ACK);
    i2c_master_read_byte(handle,
                         &rx[3],
                         I2C_MASTER_NACK);/* Read the first three response bytes */ /* Read the checksum byte */

    i2c_master_stop(handle);

    ret = i2c_master_cmd_begin(
            i2c_bus_port(),
            handle,
            pdMS_TO_TICKS(1000));/* Execute the read transaction */

    i2c_cmd_link_delete(handle);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG,
                 "Read failed (%s)",
                 esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG,/* Display the received response packet for debugging */
             "RX = %02X %02X %02X %02X",
             rx[0],
             rx[1],
             rx[2],
             rx[3]);

    uint8_t checksum =
        (rx[0] + rx[1] + rx[2]) & 0xFF;

    if (checksum != rx[3])
    {
        ESP_LOGE(TAG,
                 "Checksum error calc=%02X recv=%02X",
                 checksum,
                 rx[3]);

        return ESP_ERR_INVALID_CRC;
    }

    *co2_ppm = ((uint16_t)rx[1] << 8) | rx[2];

    return ESP_OK;
}