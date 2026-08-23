#ifndef I2C_BUS_H
#define I2C_BUS_H

#include "driver/i2c.h"
#include "esp_err.h"

typedef enum
{
    I2C_BUS_OLED,
    I2C_BUS_PM,
    I2C_BUS_K30,
    I2C_BUS_RTC

} i2c_bus_device_t;

esp_err_t i2c_bus_init(void);

esp_err_t i2c_bus_select(i2c_bus_device_t device);

i2c_port_t i2c_bus_port(void);

#endif