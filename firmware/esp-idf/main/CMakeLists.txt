idf_component_register(
    SRCS "main.c"

    INCLUDE_DIRS "."

    REQUIRES
        config
        k30
        tb600b_co
        tb600b_voc
        pm_sensor
        oled
        sensor_manager
        i2c_bus
        sdcard
        tinyml
        wifi
        weather
        aqi
        interocept
        feature_builder
        ml_manager

    PRIV_REQUIRES
        spi_flash
)