/******************************************************************************
 * File Name   : tb600b_voc.c
 * Project     : PolluSense
 * Description :
 * Implements the UART driver for the TB600B Volatile Organic Compound (VOC)
 * sensor.
 *
 * This module is responsible for:
 *   - Initializing UART communication.
 *   - Sending VOC measurement requests.
 *   - Receiving sensor response frames.
 *   - Extracting the VOC concentration value.
 ******************************************************************************/

#include "tb600b_voc.h"
#include "pins.h"

#include "driver/uart.h"     /* ESP-IDF UART driver */
#include "esp_log.h"          /* ESP-IDF logging utilities */

/* UART configuration */
#define TB600B_UART_NUM      UART_NUM_2
#define TB600B_BAUDRATE      9600

/* Expected response frame length */
#define TB600B_FRAME_LEN     9

/* Logging tag */
static const char *TAG = "TB600B_VOC";

/* Read command used to request a VOC measurement */
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

/******************************************************************************
 * Function    : tb600b_voc_init
 *
 * Description :
 * Initializes UART communication with the TB600B VOC sensor.
 *
 * Parameters  : None
 *
 * Returns
 *      ESP_OK on successful initialization.
 ******************************************************************************/
esp_err_t tb600b_voc_init(void)
{
    /* UART communication configuration */
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

    /* Apply UART communication parameters */
    ESP_ERROR_CHECK(
        uart_param_config(
            TB600B_UART_NUM,
            &uart_cfg));

    /* Configure UART GPIO pins */
    ESP_ERROR_CHECK(
        uart_set_pin(
            TB600B_UART_NUM,
            TB600B_VOC_TX,
            TB600B_VOC_RX,
            UART_PIN_NO_CHANGE,
            UART_PIN_NO_CHANGE));

    /* Indicate successful initialization */
    ESP_LOGI(TAG, "TB600B VOC UART initialized");

    return ESP_OK;
}

/******************************************************************************
 * Function    : tb600b_voc_read
 *
 * Description :
 * Reads one VOC measurement from the TB600B sensor.
 *
 * Processing Steps:
 *   1. Clear the UART buffer.
 *   2. Send the VOC read command.
 *   3. Receive the response frame.
 *   4. Verify the received frame length.
 *   5. Extract the VOC concentration.
 *
 * Parameters
 *      voc_ppm : Pointer used to store the VOC concentration.
 *
 * Returns
 *      ESP_OK on success, otherwise an ESP-IDF error code.
 ******************************************************************************/
esp_err_t tb600b_voc_read(uint16_t *voc_ppm)
{
    /* Buffer for the received UART frame */
    uint8_t frame[TB600B_FRAME_LEN];

    /* Clear any previous UART data */
    uart_flush(TB600B_UART_NUM);

    /* Send VOC measurement request */
    uart_write_bytes(
        TB600B_UART_NUM,
        (const char *)read_cmd,
        sizeof(read_cmd));

    /* Receive the response frame */
    int len = uart_read_bytes(
        TB600B_UART_NUM,
        frame,
        TB600B_FRAME_LEN,
        pdMS_TO_TICKS(500));

    /* Verify that a complete response frame was received */
    if (len != TB600B_FRAME_LEN)
    {
        ESP_LOGE(TAG, "Expected 9 bytes, got %d", len);
        return ESP_FAIL;
    }

    /* Display the received frame for debugging */
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

    /* Extract VOC concentration from the response frame */
    *voc_ppm = ((uint16_t)frame[2] << 8) | frame[3];

    /* Display the measured VOC concentration */
    ESP_LOGI(TAG, "VOC = %u ppm", *voc_ppm);

    return ESP_OK;
}