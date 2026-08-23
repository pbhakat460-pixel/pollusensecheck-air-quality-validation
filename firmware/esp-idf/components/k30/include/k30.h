#ifndef K30_H
#define K30_H

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t k30_init(void);

esp_err_t k30_read(uint16_t *co2_ppm);

#ifdef __cplusplus
}
#endif

#endif