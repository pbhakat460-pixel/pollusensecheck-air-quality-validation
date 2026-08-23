#ifndef CO_SCALER_H
#define CO_SCALER_H

#define NUM_FEATURES 10

static const float CO_MIN[NUM_FEATURES] = {
    8.35000000f,
    4.88000000f,
    0.01000000f,
    0.01000000f,
    0.00000000f,
    841.50000000f,
    1.00000000f,
    0.00000000f,
    0.00000000f,
    0.00000000f
};

static const float CO_MAX[NUM_FEATURES] = {
    55.98000000f,
    99.99000000f,
    29.17000000f,
    359.98000000f,
    3500.40000000f,
    1056.90000000f,
    12.00000000f,
    6.00000000f,
    23.00000000f,
    3.00000000f
};

static inline void scale_CO(float x[NUM_FEATURES])
{
    for(int i=0;i<NUM_FEATURES;i++)
    {
        x[i] = (x[i] - CO_MIN[i]) /
               (CO_MAX[i] - CO_MIN[i]);
    }
}

#endif
