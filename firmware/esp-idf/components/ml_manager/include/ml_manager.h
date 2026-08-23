#pragma once

#include "esp_err.h"
#include "feature_builder.h"
#include "ml_preprocessing.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * ML Prediction Result
 ******************************************************************************/

typedef struct
{
    /* Predicted AQI Class Indices
     *
     * PM2.5 :
     *   0 -> Good
     *   1 -> Satisfactory
     *   2 -> Moderate
     *   3 -> Poor
     *   4 -> Very Poor
     *
     * CO :
     *   0 -> Good
     *   1 -> Satisfactory
     *   2 -> Moderate
     */

    uint8_t predicted_pm25_class;

    uint8_t predicted_co_class;

} ml_prediction_t;

/******************************************************************************
 * ML Manager Initialization
 ******************************************************************************/

esp_err_t ml_manager_init(void);

/******************************************************************************
 * Perform PM2.5 AQI and CO AQI Classification
 ******************************************************************************/

esp_err_t ml_manager_predict(

    const feature_vector_t *features,

    ml_prediction_t *prediction

);

#ifdef __cplusplus
}
#endif