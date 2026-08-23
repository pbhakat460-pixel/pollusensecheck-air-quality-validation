#ifndef TB600B_VOC_H
#define TB600B_VOC_H

#include "esp_err.h"
#include <stdint.h>

esp_err_t tb600b_voc_init(void);
esp_err_t tb600b_voc_read(uint16_t *voc_ppm);

#endif