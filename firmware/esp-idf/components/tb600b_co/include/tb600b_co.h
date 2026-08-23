#ifndef TB600B_CO_H
#define TB600B_CO_H

#include "esp_err.h"
#include <stdint.h>

typedef struct
{
    uint16_t concentration_raw_mgm3;
    uint16_t concentration_raw_ppm;

    float mgm3;
    float ppm;

    uint16_t range_ppm;
    uint8_t decimal_places;

} tb600b_co_data_t;

esp_err_t tb600b_co_init(void);

esp_err_t tb600b_co_read(tb600b_co_data_t *data);

#endif