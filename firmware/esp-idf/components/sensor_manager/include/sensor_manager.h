#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"
#include "aqi.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Algorithm 2 (Interocept) Parameters
 ******************************************************************************/

#define PM_ALPHA   1.0f
#define PM_BETA    0.95f
#define PM_TAU     2
#define PM_THETA   10.0f

#define CO_ALPHA   1.0f
#define CO_BETA    0.95f
#define CO_TAU     2
#define CO_THETA   10.0f

/******************************************************************************
 * Sensor Health
 ******************************************************************************/

typedef enum
{
    SENSOR_HEALTHY = 0,
    SENSOR_FAULTY

} sensor_health_t;

/******************************************************************************
 * Sensor Data
 ******************************************************************************/

typedef struct
{
    /**************** PM Sensor ****************/

    uint16_t pm1;

    uint16_t pm25;

    uint16_t pm10;

    uint16_t particle03;

    uint16_t particle05;

    uint16_t particle10;

    uint16_t particle25;

    uint16_t particle50;

    uint16_t particle100;

    /**************** CO Sensor ****************/

    float co_mgm3;

    float co_ppm;

    /**************** VOC ****************/

    uint16_t voc;

    /**************** CO2 ****************/

    uint16_t co2;

    /**************** AQI ****************/

    float pm25_aqi;

    float co_aqi;

    aqi_class_t pm25_class;

    aqi_class_t co_class;

    /**************** Algorithm 2 Fault Scores ****************/

    float pm_fault_score;

    float co_fault_score;
        /**************** Sensor Health ****************/

    sensor_health_t pm_sensor_status;

    sensor_health_t co_sensor_status;

    /**************** Algorithm 2 ****************/

    int pm_difference;

    int co_difference;

} sensor_data_t;

/******************************************************************************
 * Initialize Sensors
 ******************************************************************************/

esp_err_t sensor_manager_init(void);

/******************************************************************************
 * Read Sensors
 ******************************************************************************/

esp_err_t sensor_manager_read(
    sensor_data_t *data
);

/******************************************************************************
 * Calculate AQI
 ******************************************************************************/

esp_err_t sensor_manager_calculate_aqi(
    sensor_data_t *data
);

/******************************************************************************
 * Compare Prediction and Sensor AQI
 ******************************************************************************/

esp_err_t sensor_manager_compare(

    sensor_data_t *sensor,

    aqi_class_t predicted_pm_class,

    aqi_class_t predicted_co_class

);

#ifdef __cplusplus
}
#endif

#endif
