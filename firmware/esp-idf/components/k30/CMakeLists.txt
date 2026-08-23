idf_component_register(
    SRCS
        "k30.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        driver
        esp_driver_i2c
        config
        i2c_bus
)