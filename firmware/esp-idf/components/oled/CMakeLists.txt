idf_component_register(

    SRCS
        "oled.c"
        "ssd1306.c"
        "ssd1306_i2c_legacy.c"
        "ssd1306_spi.c"

    INCLUDE_DIRS
        "."
        "include"

    REQUIRES
        driver
        esp_driver_i2c
        esp_driver_spi
        config
        i2c_bus
        sensor_manager
        ml_manager

)