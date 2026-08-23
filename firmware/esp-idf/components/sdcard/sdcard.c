/******************************************************************************
 * File Name   : sdcard.c
 * Project     : PolluSense
 * Description :
 * Implements SD card storage support for the PolluSense system.
 *
 * This module is responsible for:
 *   - Initializing the SPI interface for the SD card.
 *   - Mounting the FAT filesystem.
 *   - Creating the sensor data log file.
 *   - Logging real-time sensor readings into a CSV file.
 ******************************************************************************/

#include "sdcard.h"              /* SD card interface */
#include "sensor_manager.h"      /* Sensor data structure definitions */
#include "esp_log.h"             /* ESP-IDF logging utilities */
#include <stdio.h>               /* Standard file I/O functions */
#include <time.h>
#include "driver/spi_master.h"   /* SPI master driver */

#include "esp_vfs_fat.h"         /* FAT filesystem support */

#include "sdmmc_cmd.h"           /* SD/MMC card driver */

#include "pins.h"                /* Hardware pin definitions */

/* Logging tag for the SD card module */
static const char *TAG = "SDCARD";

/* Pointer to the mounted SD card descriptor */
static sdmmc_card_t *card = NULL;

/* Sequential record counter for CSV logging */
static uint32_t record_no = 0;

/******************************************************************************
 * Function    : sdcard_init
 *
 * Description :
 * Initializes the SPI interface, mounts the SD card, and prepares the
 * FAT filesystem for file operations.
 *
 * Parameters  : None
 *
 * Returns
 *      ESP_OK on success, otherwise an ESP-IDF error code.
 ******************************************************************************/
esp_err_t sdcard_init(void)
{
    /* Indicate the start of SD card initialization */
    ESP_LOGI(TAG, "Initializing SD card...");

    /* Configure SPI bus parameters */
    spi_bus_config_t bus_cfg =
    {
        .mosi_io_num = SD_MOSI,
        .miso_io_num = SD_MISO,
        .sclk_io_num = SD_SCLK,

        /* Quad SPI is not used */
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,

        /* Maximum SPI transfer size */
        .max_transfer_sz = 4000,
    };

    /* Initialize the SPI bus */
    esp_err_t ret =
        spi_bus_initialize(
            SPI2_HOST,
            &bus_cfg,
            SPI_DMA_CH_AUTO);

    /* Stop initialization if SPI setup fails */
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG,
                 "SPI bus init failed (%s)",
                 esp_err_to_name(ret));
        return ret;
    }

    /* Configure SD card SPI device settings */
    sdspi_device_config_t slot_config =
        SDSPI_DEVICE_CONFIG_DEFAULT();

    slot_config.gpio_cs = SD_CS;
    slot_config.host_id = SPI2_HOST;

    /* Configure FAT filesystem mounting parameters */
    esp_vfs_fat_sdmmc_mount_config_t mount_config =
    {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    /* Configure SD card host */
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI2_HOST;

    /* Mount the SD card filesystem */
    ret = esp_vfs_fat_sdspi_mount(
            "/sdcard",
            &host,
            &slot_config,
            &mount_config,
            &card);

    /* Handle mount failure */
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG,
                 "Mount failed (%s)",
                 esp_err_to_name(ret));

        /* Release SPI resources */
        spi_bus_free(SPI2_HOST);

        return ret;
    }

    /* SD card is ready for file operations */
    ESP_LOGI(TAG, "SD Card Mounted Successfully");

    /* Display SD card information */
    sdmmc_card_print_info(stdout, card);

    return ESP_OK;
}

/******************************************************************************
 * Function    : sdcard_create_log
 *
 * Description :
 * Creates the sensor log CSV file if it does not already exist.
 *
 * Parameters  : None
 *
 * Returns
 *      ESP_OK on success.
 ******************************************************************************/
esp_err_t sdcard_create_log(void)
{
    /* Check whether the log file already exists */
    FILE *fp = fopen("/sdcard/log1.csv", "r");

    if (fp != NULL)
    {
        fclose(fp);

        ESP_LOGI(TAG, "log1.csv already exists");

        return ESP_OK;
    }

    /* Create a new log file */
    fp = fopen("/sdcard/log1.csv", "w");

    /* Stop if file creation fails */
    if (fp == NULL)
    {
        ESP_LOGE(TAG, "Unable to create log1.csv");

        return ESP_FAIL;
    }

    /* Write the CSV column headers */
    fprintf(fp,
            "Record,Date,Time,Uptime(s),"
            "PM1,PM2.5,PM10,"
            "CO_mg_m3,CO_ppm,VOC,CO2,"
            "PM_AQI,CO_AQI,"
            "PM_Class,CO_Class,"
            "PM_Difference,CO_Difference,"
            "PM_FaultScore,CO_FaultScore,"
            "PM_Status,CO_Status\n");

    /* Close the file after writing the header */
    fclose(fp);

    ESP_LOGI(TAG, "log1.csv created");

    return ESP_OK;
}

/******************************************************************************
 * Function    : sdcard_log
 *
 * Description :
 * Appends one set of environmental sensor readings to the CSV log file.
 *
 * Parameters
 *      sensor : Pointer to the latest sensor measurements.
 *
 * Returns
 *      ESP_OK on success, otherwise an ESP-IDF error code.
 ******************************************************************************/
esp_err_t sdcard_log(const sensor_data_t *sensor)
{

    /* Validate the input pointer */
    if (sensor == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    /* Open the CSV log file in append mode */
    FILE *fp = fopen("/sdcard/log1.csv", "a");

    /* Stop if the log file cannot be opened */
    if (fp == NULL)
    {
        ESP_LOGE(TAG, "Unable to open log1.csv");

        return ESP_FAIL;
    }

    /* Increment the record number */
    record_no++;
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    char date_str[16];
    char time_str[16];

    strftime(date_str,
            sizeof(date_str),
            "%d-%m-%Y",
            &timeinfo);

    strftime(time_str,
            sizeof(time_str),
            "%H:%M:%S",
            &timeinfo);

    /* Append the latest sensor readings to the CSV file */
    fprintf(fp,
            "%lu,%s,%s,%lu,"
            "%u,%u,%u,"
            "%.2f,%.2f,%u,%u,"
            "%.2f,%.2f,"
            "%d,%d,"
            "%d,%d,"
            "%.2f,%.2f,"
            "%d,%d\n",

            (unsigned long)record_no,
            date_str,
            time_str,
            (unsigned long)(esp_log_timestamp()/1000),

            sensor->pm1,
            sensor->pm25,
            sensor->pm10,

            sensor->co_mgm3,
            sensor->co_ppm,
            sensor->voc,
            sensor->co2,

            sensor->pm25_aqi,
            sensor->co_aqi,

            sensor->pm25_class,
            sensor->co_class,

            sensor->pm_difference,
            sensor->co_difference,

            sensor->pm_fault_score,
            sensor->co_fault_score,

            sensor->pm_sensor_status,
            sensor->co_sensor_status
            );

    /* Ensure all data is written to the SD card */
    fflush(fp);

    /* Close the log file */
    fclose(fp);

    /* Display the logged values for debugging */
    ESP_LOGI(TAG,
        "Logged | PMAQI=%.2f COAQI=%.2f | PMDiff=%d CO_Diff=%d | PMFP=%.2f COFP=%.2f",

        sensor->pm25_aqi,
        sensor->co_aqi,

        sensor->pm_difference,
        sensor->co_difference,

        sensor->pm_fault_score,
        sensor->co_fault_score);

    return ESP_OK;
}