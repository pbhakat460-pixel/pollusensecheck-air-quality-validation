#ifndef AQI_H
#define AQI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * AQI Class Definitions
 *
 * PM2.5 Classes
 * -------------
 * 0 : Good
 * 1 : Satisfactory
 * 2 : Moderate
 * 3 : Poor
 * 4 : Very Poor
 * 6 : Sensor Blocked (Raw PM2.5 == 0)
 *
 * CO Classes
 * ----------
 * 0 : Good
 * 1 : Satisfactory
 * 2 : Moderate
 * 4 : Sensor Blocked (Raw CO == 0)
 ******************************************************************************/

typedef enum
{
    AQI_GOOD             = 0,
    AQI_SATISFACTORY     = 1,
    AQI_MODERATE         = 2,
    AQI_POOR             = 3,
    AQI_VERY_POOR        = 4,
    AQI_SEVERE           = 5,
    AQI_SENSOR_BLOCKED   = 6

} aqi_class_t;

/******************************************************************************
 * AQI Result Structure
 ******************************************************************************/

typedef struct
{
    /* Lookup result for PM2.5 */
    float pm25_aqi;

    /* Lookup result for CO */
    float co_aqi;

    /* PM2.5 Class */
    aqi_class_t pm25_class;

    /* CO Class */
    aqi_class_t co_class;

} aqi_data_t;

/******************************************************************************
 * Lookup Functions
 *
 * NOTE:
 * These functions no longer calculate CPCB AQI.
 * They directly return the lookup-table class value as a float so the rest
 * of the project remains unchanged.
 ******************************************************************************/

/* PM2.5 (µg/m³) -> AQI Class */
float aqi_pm25(float pm25);

/* CO (mg/m³) -> AQI Class */
float aqi_co(float co);

/******************************************************************************
 * AQI Classification
 ******************************************************************************/

/* Convert lookup value to enum */
aqi_class_t pm25_aqi_class(float value);

/* Convert lookup value to enum */
aqi_class_t co_aqi_class(float value);

/******************************************************************************
 * AQI Status String
 ******************************************************************************/

const char *aqi_class_string(aqi_class_t cls);

/******************************************************************************
 * Complete AQI Calculation
 ******************************************************************************/

aqi_data_t aqi_calculate(
    float pm25,
    float co
);

#ifdef __cplusplus
}
#endif

#endif