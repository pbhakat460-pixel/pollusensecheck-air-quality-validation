/******************************************************************************
 * File Name   : aqi.c
 * Project     : PolluSense
 * Description :
 * Direct lookup table based AQI class module.
 *
 * No CPCB AQI interpolation is used.
 *
 * PM2.5 Mapping
 * -------------
 * Raw PM2.5 == 0        -> Class 6 (Sensor Blocked)
 * 1 - 50               -> Class 0 (Good)
 * 51 - 100             -> Class 1 (Satisfactory)
 * 101 - 250            -> Class 2 (Moderate)
 * 251 - 350            -> Class 3 (Poor)
 * >350                 -> Class 4 (Very Poor)
 *
 * CO Mapping
 * ----------
 * Raw CO == 0          -> Class 4 (Sensor Blocked)
 * 0 < CO <= 1.0        -> Class 0 (Good)
 * 1.0 < CO <= 2.0      -> Class 1 (Satisfactory)
 * >2.0                 -> Class 2 (Moderate)
 ******************************************************************************/

#include "aqi.h"

#include <math.h>

/******************************************************************************
 * PM2.5 Lookup
 ******************************************************************************/

float aqi_pm25(float pm25)
{
    /* Sensor blocked */
    if(pm25 <= 0.0f)
        return (float)AQI_SENSOR_BLOCKED;

    /* Good */
    if(pm25 <= 50.0f)
        return (float)AQI_GOOD;

    /* Satisfactory */
    if(pm25 <= 100.0f)
        return (float)AQI_SATISFACTORY;

    /* Moderate */
    if(pm25 <= 250.0f)
        return (float)AQI_MODERATE;

    /* Poor */
    if(pm25 <= 350.0f)
        return (float)AQI_POOR;

    /* Very Poor */
    return (float)AQI_VERY_POOR;
}

/******************************************************************************
 * CO Lookup
 ******************************************************************************/

float aqi_co(float co)
{
    /* Sensor blocked */
    if(co <= 0.0f)
        return 4.0f;

    /* Good */
    if(co <= 1.0f)
        return (float)AQI_GOOD;

    /* Satisfactory */
    if(co <= 2.0f)
        return (float)AQI_SATISFACTORY;

    /* Moderate */
    return (float)AQI_MODERATE;
}

/******************************************************************************
 * PM2.5 AQI Class
 ******************************************************************************/

aqi_class_t pm25_aqi_class(float value)
{
    return (aqi_class_t)((int)value);
}

/******************************************************************************
 * CO AQI Class
 ******************************************************************************/

aqi_class_t co_aqi_class(float value)
{
    return (aqi_class_t)((int)value);
}

/******************************************************************************
 * AQI Class String
 ******************************************************************************/

const char *aqi_class_string(aqi_class_t cls)
{
    switch(cls)
    {
        case AQI_GOOD:
            return "Good";

        case AQI_SATISFACTORY:
            return "Satisfactory";

        case AQI_MODERATE:
            return "Moderate";

        case AQI_POOR:
            return "Poor";

        case AQI_VERY_POOR:
            return "Very Poor";

        case AQI_SEVERE:
            return "Severe";

        case AQI_SENSOR_BLOCKED:
            return "Sensor Blocked";

        default:
            return "Unknown";
    }
}

/******************************************************************************
 * Complete AQI Calculation
 ******************************************************************************/

aqi_data_t aqi_calculate(
    float pm25,
    float co)
{
    aqi_data_t result;

    /*----------------------------------------------------------
      PM2.5
    ----------------------------------------------------------*/

    result.pm25_aqi = aqi_pm25(pm25);

    result.pm25_class =
        pm25_aqi_class(result.pm25_aqi);

    /*----------------------------------------------------------
      CO
    ----------------------------------------------------------*/

    result.co_aqi = aqi_co(co);

    result.co_class =
        co_aqi_class(result.co_aqi);

    return result;
}