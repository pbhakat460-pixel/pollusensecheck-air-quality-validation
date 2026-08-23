/******************************************************************************
 * File Name   : oled.c
 * Project     : PolluSense
 * Description :
 * OLED Display Manager
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "esp_log.h"

#include "oled.h"
#include "ssd1306.h"
#include "pins.h"
#include "i2c_bus.h"
#include "aqi.h"

static SSD1306_t dev;

static const char *TAG = "OLED";

/******************************************************************************
 * OLED Initialization
 ******************************************************************************/

esp_err_t oled_init(void)
{
    ESP_LOGI(TAG, "Initializing OLED");

    i2c_bus_select(I2C_BUS_OLED);

    i2c_device_add(
        &dev,
        i2c_bus_port(),
        -1);

    ssd1306_init(
        &dev,
        128,
        64);

    ssd1306_clear_screen(
        &dev,
        false);

    ESP_LOGI(TAG, "OLED Ready");

    return ESP_OK;
}

/******************************************************************************
 * Clear OLED
 ******************************************************************************/

void oled_clear(void)
{
    i2c_bus_select(I2C_BUS_OLED);

    ssd1306_clear_screen(
        &dev,
        false);
}

/******************************************************************************
 * Display Message
 ******************************************************************************/

void oled_show_message(
    const char *msg)
{
    i2c_bus_select(I2C_BUS_OLED);

    oled_clear();

    ssd1306_display_text(
        &dev,
        0,
        (char *)msg,
        strlen(msg),
        false);
}

/******************************************************************************
 * Boot Screen
 ******************************************************************************/

void oled_boot_screen(void)
{
    i2c_bus_select(I2C_BUS_OLED);

    oled_clear();

    ssd1306_display_text(
        &dev,
        0,
        "PolluSense",
        10,
        false);

    ssd1306_display_text(
        &dev,
        2,
        "Starting...",
        11,
        false);

    ssd1306_display_text(
        &dev,
        4,
        "Please Wait",
        11,
        false);
}
/******************************************************************************
 * Dashboard Screen
 ******************************************************************************/

static void oled_show_dashboard(
    const sensor_data_t *sensor,
    const weather_data_t *weather)
{
    char line[32];

    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    char time_str[16];

    strftime(
        time_str,
        sizeof(time_str),
        "%H:%M:%S",
        &timeinfo);

    i2c_bus_select(I2C_BUS_OLED);

    oled_clear();

    /**************** Title ****************/

    ssd1306_display_text(
        &dev,
        0,
        "PolluSense",
        10,
        false);

    /**************** PM Status ****************/

    sprintf(
        line,
        "PM : %s",
        sensor->pm_sensor_status ==
        SENSOR_HEALTHY ?
        "HEALTHY" :
        "FAULTY");

    ssd1306_display_text(
        &dev,
        1,
        line,
        strlen(line),
        false);

    /**************** CO Status ****************/

    sprintf(
        line,
        "CO : %s",
        sensor->co_sensor_status ==
        SENSOR_HEALTHY ?
        "HEALTHY" :
        "FAULTY");

    ssd1306_display_text(
        &dev,
        2,
        line,
        strlen(line),
        false);

    /**************** Overall AQI ****************/

    sprintf(
        line,
        "AQI: %s",
        aqi_class_string(sensor->pm25_class));

    ssd1306_display_text(
        &dev,
        4,
        line,
        strlen(line),
        false);

    /**************** Sensor Warning ****************/

    if(sensor->pm_sensor_status == SENSOR_FAULTY ||
       sensor->co_sensor_status == SENSOR_FAULTY)
    {
        ssd1306_display_text(
            &dev,
            5,
            "CHECK SENSOR",
            12,
            false);
    }

    /**************** Time ****************/

    ssd1306_display_text(
        &dev,
        7,
        time_str,
        strlen(time_str),
        false);
}
/******************************************************************************
 * OLED Update
 ******************************************************************************/

void oled_update(
    const sensor_data_t *sensor,
    const weather_data_t *weather)
{
    if(sensor == NULL)
        return;

    if(weather == NULL)
        return;

    oled_show_dashboard(
        sensor,
        weather);
}