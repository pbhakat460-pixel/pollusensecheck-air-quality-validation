/******************************************************************************
 * File Name   : feature_builder.c
 * Project     : PolluSense
 * Description :
 * Builds the 10-dimensional feature vector required by the
 * PM2.5 and CO regression MLP models.
 *
 * Feature Order
 * ----------------------------------------------------
 * [0]  AT       (Temperature)
 * [1]  RH       (Relative Humidity)
 * [2]  WS       (Wind Speed)
 * [3]  WD       (Wind Direction)
 * [4]  TOT      (Total Rainfall)
 * [5]  BP       (Barometric Pressure)
 * [6]  month
 * [7]  weekday
 * [8]  hour
 * [9]  season
 ******************************************************************************/

#include <string.h>
#include <time.h>

#include "feature_builder.h"

/******************************************************************************
 * Function    : feature_builder_create
 *
 * Description :
 * Creates the 10-dimensional feature vector used by the
 * PM2.5 and CO regression models.
 ******************************************************************************/
void feature_builder_create(feature_vector_t *fv,
                            const weather_data_t *weather)
{
    /* Clear feature vector */
    memset(fv, 0, sizeof(feature_vector_t));

    /**********************************************************
     * Weather Features
     **********************************************************/

    fv->feature[0] = weather->temperature;      /* AT  */
    fv->feature[1] = weather->humidity;         /* RH  */
    fv->feature[2] = weather->wind_speed;       /* WS  */
    fv->feature[3] = weather->wind_direction;   /* WD  */
    fv->feature[4] = weather->rainfall;         /* TOT */
    fv->feature[5] = weather->pressure;         /* BP  */

    /**********************************************************
     * Current Date & Time
     **********************************************************/

    time_t now = time(NULL);

    struct tm tm_now;

    localtime_r(&now, &tm_now);

    int month   = tm_now.tm_mon + 1;
    int weekday = tm_now.tm_wday;
    int hour    = tm_now.tm_hour;

    fv->feature[6] = month;
    fv->feature[7] = weekday;
    fv->feature[8] = hour;

    /**********************************************************
     * Season Encoding
     *
     * IMPORTANT:
     * This encoding MUST be identical to the one used while
     * training the MLP models.
     *
     * 0 = Winter
     * 1 = Summer
     * 2 = Monsoon
     * 3 = Post Monsoon
     **********************************************************/

    int season;

    if (month == 12 || month == 1 || month == 2)
    {
        season = 0;
    }
    else if (month >= 3 && month <= 5)
    {
        season = 1;
    }
    else if (month >= 6 && month <= 9)
    {
        season = 2;
    }
    else
    {
        season = 3;
    }

    fv->feature[9] = season;
}