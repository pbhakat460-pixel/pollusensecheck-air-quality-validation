#ifndef FEATURE_BUILDER_H
#define FEATURE_BUILDER_H

#include <stdint.h>
#include "weather.h"

/*
 * Number of input features expected by the MLP model.
 *
 * Feature Order:
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
 * ----------------------------------------------------
 */

#define FEATURE_COUNT 10

typedef struct
{
    float feature[FEATURE_COUNT];

} feature_vector_t;

/*
 * Creates the 10-dimensional feature vector used by the
 * PM2.5 and CO regression MLP models.
 */
void feature_builder_create(feature_vector_t *fv,
                            const weather_data_t *weather);

#endif