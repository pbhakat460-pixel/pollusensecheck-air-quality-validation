idf_component_register(
    SRCS "tb600b_co.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_uart config
)