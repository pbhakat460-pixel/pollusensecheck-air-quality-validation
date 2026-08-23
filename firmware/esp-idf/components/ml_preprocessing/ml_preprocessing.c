/******************************************************************************
 * File Name   : ml_preprocessing.c
 * Project     : PolluSense
 * Description :
 * Implements preprocessing before TinyML classification inference.
 *
 * The preprocessing pipeline consists only of:
 *      1. Min-Max Feature Scaling
 *
 * No PCA is used.
 ******************************************************************************/

#include "ml_preprocessing.h"

/******************************************************************************
 * Function    : ml_scale_features
 *
 * Description :
 * Applies Min-Max scaling to all input features.
 *
 * Formula:
 *
 *      scaled = (input - min) / (max - min)
 ******************************************************************************/

void ml_scale_features(

    const feature_vector_t *input,

    const float *featureMin,

    const float *featureMax,

    ml_input_t *output

)
{
    for(int i = 0; i < FEATURE_COUNT; i++)
    {
        output->feature[i] =

            (input->feature[i] - featureMin[i]) /

            (featureMax[i] - featureMin[i]);
    }
}

/******************************************************************************
 * Function    : ml_preprocess
 *
 * Description :
 * Complete preprocessing before inference.
 ******************************************************************************/

void ml_preprocess(

    const feature_vector_t *input,

    const float *featureMin,

    const float *featureMax,

    ml_input_t *output

)
{
    ml_scale_features(

        input,

        featureMin,

        featureMax,

        output

    );
}