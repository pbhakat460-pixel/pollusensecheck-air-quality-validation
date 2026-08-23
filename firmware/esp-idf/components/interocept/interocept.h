#ifndef INTEROCEPT_H
#define INTEROCEPT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    float alpha;
    float beta;

    uint8_t tau;
    float theta;

    float fault_score;

    bool faulty;

} interocept_t;

void interocept_init(interocept_t *ctx,
                     float alpha,
                     float beta,
                     uint8_t tau,
                     float theta);

void interocept_update(interocept_t *ctx,
                       uint8_t actual_class,
                       uint8_t predicted_class);

float interocept_fault_score(const interocept_t *ctx);

bool interocept_is_faulty(const interocept_t *ctx);

#endif