#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * TinyML Configuration
 * ============================================================ */

/* Number of input features expected by both models */
#define FEATURE_SIZE              10

/* Maximum output size (PM = 5 classes, CO = 3 classes) */
#define MODEL_OUTPUT_SIZE         5

/* Tensor arena size */
#define PM_TENSOR_ARENA_SIZE      (96 * 1024)
#define CO_TENSOR_ARENA_SIZE      (96 * 1024)

/* ============================================================
 * Model Selection
 * ============================================================ */

typedef enum
{
    MODEL_PM = 0,
    MODEL_CO

} tinyml_model_t;

/* ============================================================
 * TinyML Context
 * ============================================================ */

typedef struct
{
    bool initialized;

    tinyml_model_t model;

} tinyml_context_t;

/* ============================================================
 * API
 * ============================================================ */

/* Initialize TinyML engine */
esp_err_t tinyml_init(void);

/* Load PM2.5 or CO classification model */
esp_err_t tinyml_load_model(
    tinyml_model_t model);

/* Perform inference */
esp_err_t tinyml_predict(

    const float input[FEATURE_SIZE],

    uint8_t *predicted_class

);

/* Release resources */
void tinyml_deinit(void);

#ifdef __cplusplus
}
#endif