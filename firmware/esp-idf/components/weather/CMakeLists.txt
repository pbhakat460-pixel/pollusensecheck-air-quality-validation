idf_component_register(
    SRCS
        "weather.c"
        "weather_parser.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        esp_http_client
        esp-tls
        mbedtls
        wifi
)