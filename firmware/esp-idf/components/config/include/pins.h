/******************************************************************************
 * File Name   : pins.h
 * Project     : PolluSense
 * Description :
 * Central hardware pin configuration for the PolluSense system.
 *
 * This file defines GPIO assignments and device addresses for all
 * peripherals connected to the ESP32-S3, including:
 *   - K30 CO2 sensor
 *   - TB600B CO sensor
 *   - TB600B VOC sensor
 *   - PMSA7003T particulate matter sensor
 *   - OLED display
 *   - SD card interface
 *
 * Modifying this file changes the hardware pin mapping used
 * throughout the project.
 ******************************************************************************/
#ifndef PINS_H
#define PINS_H
/* ESP-IDF GPIO definitions */
#include "driver/gpio.h"

/* ============================================================
 * K30 CO2 Sensor (I2C)
 * ============================================================ */
#define K30_I2C_SDA            GPIO_NUM_10 /* I2C Serial Data (SDA) pin for K30 CO2 sensor */
#define K30_I2C_SCL            GPIO_NUM_46 /* I2C Serial Clock (SCL) pin for K30 CO2 sensor */
#define K30_I2C_ADDRESS        0x68 /* Default I2C device address of the K30 CO2 sensor */


/* ============================================================
 * TB600B CO Sensor (UART)
 * ============================================================ */
#define TB600B_CO_TX           GPIO_NUM_17 /* UART transmit pin connected to the TB600B CO sensor */
#define TB600B_CO_RX           GPIO_NUM_9 /* UART receive pin connected to the TB600B CO sensor */

/* ============================================================
 * TB600B VOC Sensor (UART)
 * ============================================================ */
#define TB600B_VOC_TX          GPIO_NUM_16 /* UART transmit pin connected to the TB600B VOC sensor */
#define TB600B_VOC_RX          GPIO_NUM_18 /* UART receive pin connected to the TB600B VOC sensor */

/* ============================================================
 * OLED Display (I2C)
 * ============================================================ */
#define OLED_I2C_SDA           GPIO_NUM_6 /* I2C Serial Data (SDA) pin for OLED display */
#define OLED_I2C_SCL           GPIO_NUM_7 /* I2C Serial Clock (SCL) pin for OLED display */
#define OLED_I2C_ADDRESS       0x3C /* I2C address of the OLED display */

/* ============================================================
 * SD Card (SPI)
 * ============================================================ */
#define SD_MOSI                GPIO_NUM_13 /* SPI Master-Out-Slave-In (MOSI) pin for SD card */
#define SD_MISO                GPIO_NUM_11 /* SPI Master-In-Slave-Out (MISO) pin for SD card */
#define SD_SCLK                GPIO_NUM_12 /* SPI clock pin for SD card communication */
#define SD_CS                  GPIO_NUM_14 /* SPI chip-select pin for SD card */

/* ============================================================
 * PMSA7003T / PM Sensor (I2C)
 * ============================================================ */
#define PM_I2C_SDA      GPIO_NUM_4 /* I2C Serial Data (SDA) pin for PMSA7003T sensor */
#define PM_I2C_SCL      GPIO_NUM_5 /* I2C Serial Clock (SCL) pin for PMSA7003T sensor */
#define PM_I2C_ADDRESS  0x19 /* Default I2C address of the PMSA7003T sensor */

#endif