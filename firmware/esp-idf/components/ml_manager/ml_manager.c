/******************************************************************************
 * File Name   : ml_manager.c
 * Project     : PolluSense
 * Description :
 * TinyML Manager
 *
 * Pipeline
 *
 * Feature Vector
 *        ↓
 * Min-Max Scaling
 *        ↓
 * PM Classification Model
 *        ↓
 * PM AQI Class
 *
 * Feature Vector
 *        ↓
 * Min-Max Scaling
 *        ↓
 * CO Classification Model
 *        ↓
 * CO AQI Class
 ******************************************************************************/

#include "ml_manager.h"

#include "ml_preprocessing.h"
#include "tinyml.h"

#include "pm_scaler.h"
#include "co_scaler.h"

#include "esp_log.h"

#include <string.h>

static const char *TAG = "ML_MANAGER";

/******************************************************************************
 * Initialization Flag
 ******************************************************************************/

static bool initialized = false;

/******************************************************************************
 * Initialize ML Manager
 ******************************************************************************/

esp_err_t ml_manager_init(void)
{
    if(initialized)
    {
        return ESP_OK;
    }

    ESP_ERROR_CHECK(tinyml_init());

    initialized = true;

    ESP_LOGI(TAG, "ML Manager Initialized");

    return ESP_OK;
}

/******************************************************************************
 * Perform AQI Classification
 ******************************************************************************/

esp_err_t ml_manager_predict(
    const feature_vector_t *features,
    ml_prediction_t *prediction)
{
    if(!initialized)
    {
        return ESP_FAIL;
    }

    memset(prediction, 0, sizeof(ml_prediction_t));

    /******************************************************************
     * PM2.5 PREPROCESSING
     ******************************************************************/

    ml_input_t pm_input;

    ml_preprocess(

        features,

        PM_MIN,

        PM_MAX,

        &pm_input

    );

    ESP_LOGI(TAG, "========== PM INPUT ==========");

    for(int i = 0; i < FEATURE_COUNT; i++)
    {
        ESP_LOGI(
            TAG,
            "PM Feature[%02d] = %.6f",
            i,
            pm_input.feature[i]
        );
    }

    ESP_LOGI(TAG, "==============================");

    /******************************************************************
     * PM MODEL
     ******************************************************************/

    ESP_ERROR_CHECK(

        tinyml_load_model(
            MODEL_PM)

    );

    ESP_ERROR_CHECK(

        tinyml_predict(

            pm_input.feature,

            &prediction->predicted_pm25_class

        )

    );

    /******************************************************************
     * CO PREPROCESSING
     ******************************************************************/

    ml_input_t co_input;

    ml_preprocess(

        features,

        CO_MIN,

        CO_MAX,

        &co_input

    );

    ESP_LOGI(TAG, "========== CO INPUT ==========");

    for(int i = 0; i < FEATURE_COUNT; i++)
    {
        ESP_LOGI(
            TAG,
            "CO Feature[%02d] = %.6f",
            i,
            co_input.feature[i]
        );
    }

    ESP_LOGI(TAG, "==============================");

    /******************************************************************
     * CO MODEL
     ******************************************************************/

    ESP_ERROR_CHECK(

        tinyml_load_model(
            MODEL_CO)

    );

    ESP_ERROR_CHECK(

        tinyml_predict(

            co_input.feature,

            &prediction->predicted_co_class

        )

    );
        /******************************************************************
     * Prediction Summary
     ******************************************************************/

    ESP_LOGI(TAG, "========== PREDICTION ==========");

    /**************** PM2.5 ****************/

    ESP_LOGI(
        TAG,
        "Predicted PM2.5 AQI Class : %u",
        prediction->predicted_pm25_class
    );

    switch(prediction->predicted_pm25_class)
    {
        case 0:

            ESP_LOGI(TAG, "PM2.5 Category : Good");

            break;

        case 1:

            ESP_LOGI(TAG, "PM2.5 Category : Satisfactory");

            break;

        case 2:

            ESP_LOGI(TAG, "PM2.5 Category : Moderate");

            break;

        case 3:

            ESP_LOGI(TAG, "PM2.5 Category : Poor");

            break;

        case 4:

            ESP_LOGI(TAG, "PM2.5 Category : Very Poor");

            break;

        default:

            ESP_LOGW(TAG, "PM2.5 Category : Unknown");

            break;
    }

    /**************** CO ****************/

    ESP_LOGI(
        TAG,
        "Predicted CO AQI Class : %u",
        prediction->predicted_co_class
    );

    switch(prediction->predicted_co_class)
    {
        case 0:

            ESP_LOGI(TAG, "CO Category : Good");

            break;

        case 1:

            ESP_LOGI(TAG, "CO Category : Satisfactory");

            break;

        case 2:

            ESP_LOGI(TAG, "CO Category : Moderate");

            break;

        default:

            ESP_LOGW(TAG, "CO Category : Unknown");

            break;
    }

    ESP_LOGI(TAG, "===============================");

    return ESP_OK;
}