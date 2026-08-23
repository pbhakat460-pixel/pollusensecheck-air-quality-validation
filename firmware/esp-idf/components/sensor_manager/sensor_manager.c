/******************************************************************************
 * File Name   : sensor_manager.c
 * Project     : PolluSense
 * Description :
 * Sensor Manager
 *
 * Responsibilities
 *
 * 1. Initialize sensors
 * 2. Read sensors
 * 3. Calculate AQI
 * 4. Compare prediction and sensor AQI
 ******************************************************************************/

#include "sensor_manager.h"
#include <stdlib.h>
#include "pm_sensor.h"
#include "tb600b_co.h"
#include "tb600b_voc.h"
//#include "k30.h"

#include "aqi.h"

#include "esp_log.h"

static const char *TAG = "SENSOR_MANAGER";

/******************************************************************************
 * Initialize Sensors
 ******************************************************************************/

esp_err_t sensor_manager_init(void)
{
    ESP_LOGI(TAG,"Initializing Sensors");

    ESP_ERROR_CHECK(pm_sensor_init());
    ESP_ERROR_CHECK(pm_sensor_awake());

    ESP_ERROR_CHECK(tb600b_co_init());

    ESP_ERROR_CHECK(tb600b_voc_init());

    /*
    esp_err_t ret = k30_init();

    if(ret != ESP_OK)
    {
        ESP_LOGW(TAG,"K30 disabled");
    }
    */

    ESP_LOGI(TAG,"Sensors Initialized");

    return ESP_OK;
}

/******************************************************************************
 * Read Sensors
 ******************************************************************************/

esp_err_t sensor_manager_read(sensor_data_t *data)
{
    if(data == NULL)
        return ESP_ERR_INVALID_ARG;

    esp_err_t ret;

    /**************** PM ****************/

    ret = pm_sensor_read_u16(
            PARTICLE_PM1_0_ATMOSPHERE,
            &data->pm1);

    if(ret != ESP_OK)
        return ret;

    ret = pm_sensor_read_u16(
            PARTICLE_PM2_5_ATMOSPHERE,
            &data->pm25);

    if(ret != ESP_OK)
        return ret;

    ret = pm_sensor_read_u16(
            PARTICLE_PM10_ATMOSPHERE,
            &data->pm10);

    if(ret != ESP_OK)
        return ret;

    /**************** CO ****************/

    tb600b_co_data_t co;

    ret = tb600b_co_read(&co);

    if(ret != ESP_OK)
        return ret;

    data->co_mgm3 = co.mgm3;
    data->co_ppm  = co.ppm;

    /**************** VOC ****************/

    ret = tb600b_voc_read(&data->voc);

    if(ret != ESP_OK)
        return ret;

    /**************** CO2 ****************/

    data->co2 = 0;

    return ESP_OK;
}

/******************************************************************************
 * Calculate AQI
 ******************************************************************************/

esp_err_t sensor_manager_calculate_aqi(
    sensor_data_t *data)
{
    if(data == NULL)
        return ESP_ERR_INVALID_ARG;

    data->pm25_aqi =
        aqi_pm25((float)data->pm25);

    data->co_aqi =
        aqi_co(data->co_mgm3);

    data->pm25_class =
        pm25_aqi_class(
            data->pm25_aqi);

    data->co_class =
        co_aqi_class(
            data->co_aqi);

    return ESP_OK;
}
/******************************************************************************
 * Compare Predicted AQI Class with Sensor AQI Class
 ******************************************************************************/

esp_err_t sensor_manager_compare(

    sensor_data_t *sensor,

    aqi_class_t predicted_pm_class,

    aqi_class_t predicted_co_class

)
{
    if(sensor == NULL)
        return ESP_ERR_INVALID_ARG;

    /**********************************************************************
     * PM2.5 - Interocept Algorithm
     **********************************************************************/

    sensor->pm_difference =
        abs((int)predicted_pm_class -
            (int)sensor->pm25_class);

    if(sensor->pm_difference >= PM_TAU)
    {
        sensor->pm_fault_score =
            (PM_ALPHA * sensor->pm_fault_score) + 1.0f;
    }
    else
    {
        sensor->pm_fault_score =
            PM_BETA * sensor->pm_fault_score;
    }

    if(sensor->pm_fault_score >= PM_THETA)
    {
        sensor->pm_sensor_status = SENSOR_FAULTY;
    }
    else
    {
        sensor->pm_sensor_status = SENSOR_HEALTHY;
    }

    /**********************************************************************
     * CO - Interocept Algorithm
     **********************************************************************/

    sensor->co_difference =
        abs((int)predicted_co_class -
            (int)sensor->co_class);

    if(sensor->co_difference >= CO_TAU)
    {
        sensor->co_fault_score =
            (CO_ALPHA * sensor->co_fault_score) + 1.0f;
    }
    else
    {
        sensor->co_fault_score =
            CO_BETA * sensor->co_fault_score;
    }

    if(sensor->co_fault_score >= CO_THETA)
    {
        sensor->co_sensor_status = SENSOR_FAULTY;
    }
    else
    {
        sensor->co_sensor_status = SENSOR_HEALTHY;
    }

    /**********************************************************************
     * Print Results
     **********************************************************************/

    ESP_LOGI(TAG, "========== SENSOR STATUS ==========");

    ESP_LOGI(TAG, "PM2.5 Sensor");

    ESP_LOGI(TAG,
             "  Concentration : %u ug/m3",
             sensor->pm25);

    ESP_LOGI(TAG,
             "  AQI           : %.2f",
             sensor->pm25_aqi);

    ESP_LOGI(TAG,
             "  AQI Class     : %s",
             aqi_class_string(sensor->pm25_class));

    ESP_LOGI(TAG,
             "  Prediction    : %s",
             aqi_class_string(predicted_pm_class));

    ESP_LOGI(TAG,
             "  Difference    : %d",
             sensor->pm_difference);

    ESP_LOGI(TAG,
             "  Fault Score   : %.2f",
             sensor->pm_fault_score);

    ESP_LOGI(TAG,
             "  Status        : %s",
             sensor->pm_sensor_status == SENSOR_HEALTHY ?
             "HEALTHY" : "FAULTY");

    ESP_LOGI(TAG,
             "-----------------------------------");

    ESP_LOGI(TAG,
             "CO Sensor");

    ESP_LOGI(TAG,
             "  Concentration : %.2f mg/m3",
             sensor->co_mgm3);

    ESP_LOGI(TAG,
             "  AQI           : %.2f",
             sensor->co_aqi);

    ESP_LOGI(TAG,
             "  AQI Class     : %s",
             aqi_class_string(sensor->co_class));

    ESP_LOGI(TAG,
             "  Prediction    : %s",
             aqi_class_string(predicted_co_class));

    ESP_LOGI(TAG,
             "  Difference    : %d",
             sensor->co_difference);

    ESP_LOGI(TAG,
             "  Fault Score   : %.2f",
             sensor->co_fault_score);

    ESP_LOGI(TAG,
             "  Status        : %s",
             sensor->co_sensor_status == SENSOR_HEALTHY ?
             "HEALTHY" : "FAULTY");

    ESP_LOGI(TAG,
             "===================================");

    return ESP_OK;
}