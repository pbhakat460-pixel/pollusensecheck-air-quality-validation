#ifndef ML_PREPROCESSING_H
#define ML_PREPROCESSING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "feature_builder.h"

/******************************************************************************
 * Input to the TinyML Classification Models.
 *
 * Both PM2.5 AQI and CO AQI models expect exactly 10 scaled features:
 *
 * [0] AT
 * [1] RH
 * [2] WS
 * [3] WD
 * [4] TOT-RF
 * [5] BP
 * [6] month
 * [7] weekday
 * [8] hour
 * [9] season
 ******************************************************************************/

typedef struct
{
    float feature[FEATURE_COUNT];

} ml_input_t;

/******************************************************************************
 * Performs Min-Max Scaling.
 *
 * input      : Raw feature vector
 * featureMin : Minimum feature values
 * featureMax : Maximum feature values
 * output     : Scaled feature vector
 ******************************************************************************/

void ml_scale_features(

    const feature_vector_t *input,

    const float *featureMin,

    const float *featureMax,

    ml_input_t *output

);

/******************************************************************************
 * Complete preprocessing.
 *
 * Currently performs only Min-Max scaling.
 ******************************************************************************/

void ml_preprocess(

    const feature_vector_t *input,

    const float *featureMin,

    const float *featureMax,

    ml_input_t *output

);

#ifdef __cplusplus
}
#endif

#endif