/******************************************************************************
 * File Name   : tb600b_co.c
 * Project     : PolluSense
 * Description :
 * Implements the UART driver for the TB600B Carbon Monoxide (CO) sensor.
 *
 * This module is responsible for:
 *   - Initializing UART communication.
 *   - Reading sensor configuration parameters.
 *   - Sending CO measurement requests.
 *   - Receiving and validating sensor response frames.
 *   - Verifying frame checksum.
 *   - Converting raw sensor values into engineering units.
 ******************************************************************************/

#include "tb600b_co.h"
#include "pins.h"

#include "driver/uart.h"         /* ESP-IDF UART driver */
#include "esp_log.h"             /* ESP-IDF logging utilities */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Logging tag */
static const char *TAG = "TB600B_CO";

/* UART configuration */
#define TB600B_UART_NUM      UART_NUM_1
#define TB600B_BAUDRATE      9600
#define TB600B_FRAME_LEN     9

/* Command used to request a CO measurement from the sensor */
static const uint8_t read_cmd[9] =
{
    0xFF,
    0x01,
    0x86,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x79
};

/* Command used to read the sensor configuration parameters */
static const uint8_t parameter_cmd[1] =
{
    0xD1
};

/* Sensor configuration obtained during initialization */
static uint16_t sensor_range = 1000;
static uint8_t decimal_places = 3;

/******************************************************************************
 * Function    : tb600b_read_parameters
 *
 * Description :
 * Reads the sensor configuration including measurement range and decimal
 * precision.
 *
 * Parameters  : None
 *
 * Returns
 *      ESP_OK on success, otherwise ESP_FAIL.
 ******************************************************************************/
static esp_err_t tb600b_read_parameters(void)
{
    /* Buffer for parameter response frame */
    uint8_t rx[9];

    /* Remove any stale UART data */
    uart_flush_input(TB600B_UART_NUM);

    /* Send parameter query command */
    uart_write_bytes(
        TB600B_UART_NUM,
        (const char *)parameter_cmd,
        1);

    /* Receive parameter response */
    int len =
        uart_read_bytes(
            TB600B_UART_NUM,
            rx,
            9,
            pdMS_TO_TICKS(1000));

    /* Verify response length */
    if (len != 9)
    {
        ESP_LOGW(TAG,
                 "Parameter query failed, using defaults");

        return ESP_FAIL;
    }

    /* Extract measurement range */
    sensor_range =
        ((uint16_t)rx[1] << 8) |
         rx[2];

    /* Extract number of decimal places */
    decimal_places =
        rx[7] >> 4;

    ESP_LOGI(TAG,
             "Sensor Range : %u ppm",
             sensor_range);

    ESP_LOGI(TAG,
             "Decimal Places : %u",
             decimal_places);

    return ESP_OK;
}

/******************************************************************************
 * Function    : tb600b_co_init
 *
 * Description :
 * Initializes UART communication with the TB600B CO sensor and retrieves
 * its operating parameters.
 *
 * Parameters  : None
 *
 * Returns
 *      ESP_OK on success.
 ******************************************************************************/
esp_err_t tb600b_co_init(void)
{
    /* UART communication settings */
    uart_config_t uart_cfg =
    {
        .baud_rate = TB600B_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    /* Install UART driver */
    ESP_ERROR_CHECK(
        uart_driver_install(
            TB600B_UART_NUM,
            256,
            256,
            0,
            NULL,
            0));

    /* Apply UART configuration */
    ESP_ERROR_CHECK(
        uart_param_config(
            TB600B_UART_NUM,
            &uart_cfg));

    /* Configure UART GPIO pins */
    ESP_ERROR_CHECK(
        uart_set_pin(
            TB600B_UART_NUM,
            TB600B_CO_TX,
            TB600B_CO_RX,
            UART_PIN_NO_CHANGE,
            UART_PIN_NO_CHANGE));

    ESP_LOGI(TAG, "TB600B CO UART initialized");

    /* Read sensor operating parameters */
    tb600b_read_parameters();

    return ESP_OK;
}

/******************************************************************************
 * Function    : tb600b_co_read
 *
 * Description :
 * Reads one Carbon Monoxide measurement from the TB600B sensor.
 *
 * Processing Steps:
 *   1. Send measurement request.
 *   2. Receive response frame.
 *   3. Validate frame header.
 *   4. Verify checksum.
 *   5. Decode raw values.
 *   6. Convert raw values into mg/m³ and ppm.
 *
 * Parameters
 *      data : Pointer to the output structure.
 *
 * Returns
 *      ESP_OK on success, otherwise an ESP-IDF error code.
 ******************************************************************************/
esp_err_t tb600b_co_read(tb600b_co_data_t *data)
{
    /* Validate output pointer */
    if (data == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    /* Buffer for received UART frame */
    uint8_t frame[TB600B_FRAME_LEN];

    /* Remove stale UART data */
    uart_flush_input(TB600B_UART_NUM);

    /* Send CO measurement request */
    int tx = uart_write_bytes(
        TB600B_UART_NUM,
        (const char *)read_cmd,
        sizeof(read_cmd));

    /* Ensure the complete command was transmitted */
    if (tx != sizeof(read_cmd))
    {
        ESP_LOGE(TAG, "UART write failed");
        return ESP_FAIL;
    }

    /* Receive sensor response frame */
    int len = uart_read_bytes(
        TB600B_UART_NUM,
        frame,
        TB600B_FRAME_LEN,
        pdMS_TO_TICKS(500));

    /* Verify expected frame length */
    if (len != TB600B_FRAME_LEN)
    {
        ESP_LOGE(TAG,
                 "Expected %d bytes, got %d",
                 TB600B_FRAME_LEN,
                 len);

        return ESP_FAIL;
    }

    /* Validate frame header */
    if (frame[0] != 0xFF || frame[1] != 0x86)
    {
        ESP_LOGE(TAG, "Invalid response frame");

        return ESP_FAIL;
    }

    /* Display the complete received frame for debugging */
    ESP_LOGI(TAG,
             "RX = %02X %02X %02X %02X %02X %02X %02X %02X %02X",
             frame[0],
             frame[1],
             frame[2],
             frame[3],
             frame[4],
             frame[5],
             frame[6],
             frame[7],
             frame[8]);

    /* Calculate checksum according to the TB600B communication protocol */
    uint8_t checksum =
        0xFF -
        (frame[1] +
         frame[2] +
         frame[3] +
         frame[4] +
         frame[5] +
         frame[6] +
         frame[7]) +
        1;

    /* Verify checksum integrity */
    if (checksum != frame[8])
    {
        ESP_LOGE(TAG,
                 "Checksum error calc=%02X recv=%02X",
                 checksum,
                 frame[8]);

        return ESP_ERR_INVALID_CRC;
    }

    /* Extract raw CO concentration (mg/m³) */
    uint16_t raw_mgm3 =
        ((uint16_t)frame[2] << 8) |
        frame[3];

    /* Extract raw CO concentration (ppm) */
    uint16_t raw_ppm =
        ((uint16_t)frame[6] << 8) |
        frame[7];

    /* Compute scaling factor based on decimal precision */
    float divisor = 1.0f;

    for (int i = 0; i < decimal_places; i++)
    {
        divisor *= 10.0f;
    }

    /* Store raw sensor values */
    data->concentration_raw_mgm3 = raw_mgm3;
    data->concentration_raw_ppm  = raw_ppm;

    /* Convert raw values into engineering units */
    data->mgm3 = raw_mgm3 / divisor;
    data->ppm  = raw_ppm  / divisor;

    /* Store sensor configuration information */
    data->range_ppm = sensor_range;
    data->decimal_places = decimal_places;

    /* Display final CO measurements */
    ESP_LOGI(TAG,
            "CO = %.3f mg/m3   %.3f ppm",
            data->mgm3,
            data->ppm);

    return ESP_OK;
}