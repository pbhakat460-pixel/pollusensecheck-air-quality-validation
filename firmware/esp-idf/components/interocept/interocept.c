/******************************************************************************
 * File Name   : interocept.c
 * Project     : PolluSense
 * Description :
 * Implements the Interocept sensor health monitoring algorithm.
 *
 * This module continuously compares AQI-based classifications with
 * TinyML prediction results to estimate sensor health.
 *
 * The algorithm:
 *   - Computes classification error.
 *   - Updates a cumulative fault score.
 *   - Determines whether a sensor is faulty.
 ******************************************************************************/
/* Interocept algorithm definitions */
#include "interocept.h"
/* Mathematical utility functions */
#include <math.h>
/* Standard utility functions (abs) */
#include <stdlib.h>
/******************************************************************************
 * Function    : interocept_init
 *
 * Description :
 * Initializes the Interocept monitoring context with the
 * user-defined algorithm parameters.
 *
 * Parameters
 *      ctx    : Interocept context structure
 *      alpha  : Fault score accumulation factor
 *      beta   : Fault score decay factor
 *      tau    : Error threshold
 *      theta  : Fault decision threshold
 *
 * Returns
 *      None
 ******************************************************************************/
void interocept_init(interocept_t *ctx,
                     float alpha,
                     float beta,
                     uint8_t tau,
                     float theta)
{
    ctx->alpha = alpha;/* Store Interocept configuration parameters */
    ctx->beta = beta;
    ctx->tau = tau;
    ctx->theta = theta;

    ctx->fault_score = 0.0f;/* Initialize sensor health status */
    ctx->faulty = false;
}
/******************************************************************************
 * Function    : interocept_update
 *
 * Description :
 * Updates the sensor fault score using the difference between
 * the AQI class and the TinyML predicted class.
 *
 * Parameters
 *      ctx              : Interocept context
 *      actual_class     : AQI-derived class
 *      predicted_class  : TinyML predicted class
 *
 * Returns
 *      None
 ******************************************************************************/
void interocept_update(interocept_t *ctx,
                       uint8_t actual_class,
                       uint8_t predicted_class)
{
    uint8_t error =
        abs((int)predicted_class -
            (int)actual_class);/* Calculate absolute classification error */

    if (error >= ctx->tau)/* Check whether the prediction error exceeds the allowable threshold */
    {
        ctx->fault_score =
            ctx->alpha *
            ctx->fault_score +
            1.0f;/* Increase accumulated fault score */
    }
    else
    {
        /* Gradually reduce fault score when predictions are consistent */
        ctx->fault_score =
            ctx->beta *
            ctx->fault_score;
    }
    /* Determine whether the sensor should be marked as faulty */
    ctx->faulty =
        (ctx->fault_score >=
         ctx->theta);
}
/******************************************************************************
 * Function    : interocept_fault_score
 *
 * Description :
 * Returns the current accumulated fault score.
 *
 * Parameters
 *      ctx : Interocept context
 *
 * Returns
 *      Current fault score.
 ******************************************************************************/
float interocept_fault_score(
    const interocept_t *ctx)
{
    return ctx->fault_score;/* Return the latest sensor fault score */
}
/******************************************************************************
 * Function    : interocept_is_faulty
 *
 * Description :
 * Indicates whether the sensor is currently classified as faulty.
 *
 * Parameters
 *      ctx : Interocept context
 *
 * Returns
 *      true  - Sensor is faulty.
 *      false - Sensor is operating normally.
 ******************************************************************************/
bool interocept_is_faulty(
    const interocept_t *ctx)
{
    return ctx->faulty;/* Return current sensor health status */
}