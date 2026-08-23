#ifndef PM_SENSOR_H
#define PM_SENSOR_H

#include "esp_err.h"
#include <stdint.h>

#define PARTICLE_PM1_0_STANDARD   0x05
#define PARTICLE_PM2_5_STANDARD   0x07
#define PARTICLE_PM10_STANDARD    0x09

#define PARTICLE_PM1_0_ATMOSPHERE 0x0B
#define PARTICLE_PM2_5_ATMOSPHERE 0x0D
#define PARTICLE_PM10_ATMOSPHERE  0x0F

#define PARTICLENUM_0_3           0x11
#define PARTICLENUM_0_5           0x13
#define PARTICLENUM_1_0           0x15
#define PARTICLENUM_2_5           0x17
#define PARTICLENUM_5_0           0x19
#define PARTICLENUM_10            0x1B

#define SENSOR_VERSION            0x1D

esp_err_t pm_sensor_init(void);

esp_err_t pm_sensor_awake(void);

esp_err_t pm_sensor_sleep(void);

esp_err_t pm_sensor_read_version(uint8_t *version);

esp_err_t pm_sensor_read_u16(uint8_t reg, uint16_t *value);

#endif