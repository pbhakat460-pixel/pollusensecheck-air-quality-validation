#ifndef SDCARD_H
#define SDCARD_H

#include "esp_err.h"
#include "sensor_manager.h"



#ifdef __cplusplus
extern "C" {
#endif

esp_err_t sdcard_init(void);

esp_err_t sdcard_create_log(void);
esp_err_t sdcard_log(const sensor_data_t *sensor);

#ifdef __cplusplus
}
#endif

#endif