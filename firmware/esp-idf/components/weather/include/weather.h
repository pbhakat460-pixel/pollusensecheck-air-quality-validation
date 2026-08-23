#ifndef WEATHER_H
#define WEATHER_H

#include <stdbool.h>
#include "esp_err.h"

/******************************************************************************
 * Open-Meteo Location
 *
 * Durgapur
 ******************************************************************************/

#define OPEN_METEO_LATITUDE      23.5404352f
#define OPEN_METEO_LONGITUDE     87.2892225f

/******************************************************************************
 * Weather Data Structure
 *
 * These six atmospheric parameters are used directly by the
 * PM2.5 and CO regression MLP models.
 ******************************************************************************/

typedef struct
{
    /* Air Temperature (°C) */
    float temperature;

    /* Relative Humidity (%) */
    float humidity;

    /* Surface Pressure (hPa) */
    float pressure;

    /* Wind Speed (m/s) */
    float wind_speed;

    /* Wind Direction (Degrees) */
    float wind_direction;

    /* Rainfall (mm) */
    float rainfall;

    /* Indicates whether downloaded weather data is valid */
    bool valid;

} weather_data_t;

/******************************************************************************
 * Initialize Weather Module
 ******************************************************************************/

esp_err_t weather_init(void);

/******************************************************************************
 * Download Latest Weather Data from Open-Meteo
 ******************************************************************************/

esp_err_t weather_update(void);

/******************************************************************************
 * Get Cached Weather Data
 ******************************************************************************/

weather_data_t weather_get_data(void);

#endif