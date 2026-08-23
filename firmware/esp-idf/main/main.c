/******************************************************************************
 * File Name   : main.c
 * Project     : PolluSense
 * Description :
 * Main Application
 *
 * Pipeline
 *
 * WiFi
 *      ↓
 * Weather
 *      ↓
 * Sensors
 *      ↓
 * Feature Builder
 *      ↓
 * Classification MLP
 *      ↓
 * Sensor Comparison
 *      ↓
 * OLED
 *      ↓
 * SD Card
 ******************************************************************************/

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "i2c_bus.h"
#include "wifi.h"
#include "weather.h"
#include "sensor_manager.h"
#include "feature_builder.h"
#include "ml_manager.h"
#include "oled.h"
#include "sdcard.h"

static const char *TAG = "MAIN";

/******************************************************************************
 * Main Entry
 ******************************************************************************/

void app_main(void)
{
    esp_err_t ret;

    sensor_data_t sensor = {0};

    bool sd_available = false;

    weather_data_t weather;

    feature_vector_t feature_vector;

    ml_prediction_t prediction;

    ESP_LOGI(TAG, "System Boot");

    /************************************************************
     * I2C BUS
     ************************************************************/

    ESP_ERROR_CHECK(
        i2c_bus_init());

    /************************************************************
     * OLED
     ************************************************************/

    ESP_ERROR_CHECK(
        oled_init());

    oled_boot_screen();

    /************************************************************
     * SD CARD
     ************************************************************/

    ret = sdcard_init();

    if(ret == ESP_OK)
    {
        sdcard_create_log();

        sd_available = true;

        ESP_LOGI(TAG,"SD Ready");
    }
    else
    {
        sd_available = false;

        ESP_LOGW(TAG,"SD Not Available");
    }

    /************************************************************
     * SENSOR MANAGER
     ************************************************************/

    ESP_ERROR_CHECK(
        sensor_manager_init());

    /************************************************************
     * WIFI
     ************************************************************/

    ret = wifi_init();

    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,
                 "WiFi Failed");

        return;
    }

    /************************************************************
     * WEATHER
     ************************************************************/

    ESP_ERROR_CHECK(
        weather_init());

    weather_update();

    /************************************************************
     * ML
     ************************************************************/

    ESP_ERROR_CHECK(
        ml_manager_init());

    ESP_LOGI(TAG,
             "Initialization Complete");
        while(true)
    {
        /************************************************************
         * Update Weather
         ************************************************************/

        static uint32_t weather_counter = 0;

        if(weather_counter == 0)
        {
            ret = weather_update();

            if(ret != ESP_OK)
            {
                ESP_LOGW(TAG, "Using Cached Weather");
            }
        }

        weather = weather_get_data();

        weather_counter++;

        if(weather_counter >= 120)
        {
            weather_counter = 0;
        }

        /************************************************************
         * Read Sensors
         ************************************************************/

        ret = sensor_manager_read(
            &sensor);

        if(ret != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Sensor Read Failed");

            vTaskDelay(
                pdMS_TO_TICKS(5000));

            continue;
        }

        /************************************************************
         * Calculate AQI
         ************************************************************/

        ESP_ERROR_CHECK(
            sensor_manager_calculate_aqi(
                &sensor));

        /************************************************************
         * Build Feature Vector
         ************************************************************/

        feature_builder_create(

            &feature_vector,

            &weather

        );

        /************************************************************
         * TinyML AQI Classification
         ************************************************************/

        ret = ml_manager_predict(

            &feature_vector,

            &prediction

        );

        if(ret != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "Prediction Failed");

            vTaskDelay(
                pdMS_TO_TICKS(5000));

            continue;
        }

        /************************************************************
         * Compare Prediction with Sensors
         ************************************************************/

        ESP_ERROR_CHECK(

            sensor_manager_compare(

                &sensor,

                prediction.predicted_pm25_class,

                prediction.predicted_co_class

            )

        );

        /************************************************************
         * OLED Update
         ************************************************************/

        oled_update(
            &sensor,
            &weather
        );
                /************************************************************
         * Serial Debug Output
         ************************************************************/

        ESP_LOGI(TAG,
                 "====================================");

        ESP_LOGI(TAG,
                 "PM2.5");

        ESP_LOGI(TAG,
                 "  Real Value      : %u",
                 sensor.pm25);

        ESP_LOGI(TAG,
                 "  Real AQI        : %.2f",
                 sensor.pm25_aqi);

        ESP_LOGI(TAG,
                 "  Predicted Class : %u",
                 prediction.predicted_pm25_class + 1);

        ESP_LOGI(TAG,
                 "  Status          : %s",

                 sensor.pm_sensor_status == SENSOR_HEALTHY ?

                 "HEALTHY"

                 :

                 "FAULTY");

        ESP_LOGI(TAG,
                 "------------------------------------");

        ESP_LOGI(TAG,
                 "CO");

        ESP_LOGI(TAG,
                 "  Real Value      : %.2f",
                 sensor.co_mgm3);

        ESP_LOGI(TAG,
                 "  Real AQI        : %.2f",
                 sensor.co_aqi);

        ESP_LOGI(TAG,
                 "  Predicted Class : %u",
                 prediction.predicted_co_class + 1);

        ESP_LOGI(TAG,
                 "  Status          : %s",

                 sensor.co_sensor_status == SENSOR_HEALTHY ?

                 "HEALTHY"

                 :

                 "FAULTY");

        ESP_LOGI(TAG,
                 "====================================");

        /************************************************************
         * SD Card Logging
         ************************************************************/

        if(sd_available)
        {
            ret = sdcard_log(&sensor);

            if(ret != ESP_OK)
            {
                ESP_LOGW(TAG,
                        "SD Logging Failed");
            }
        }

        /************************************************************
         * Next Cycle
         ************************************************************/

        vTaskDelay(
            pdMS_TO_TICKS(5000));
    }
}