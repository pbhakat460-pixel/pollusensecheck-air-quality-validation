idf_component_register(
    SRCS "pm_sensor.c"
    INCLUDE_DIRS "include"
    REQUIRES
        driver
        config
        i2c_bus
)