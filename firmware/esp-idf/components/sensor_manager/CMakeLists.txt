idf_component_register(

    SRCS
        "sensor_manager.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        pm_sensor
        tb600b_co
        tb600b_voc
        k30
        aqi

)