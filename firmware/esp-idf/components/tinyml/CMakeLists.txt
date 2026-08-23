idf_component_register(

    SRCS
        "tinyml.cpp"

    INCLUDE_DIRS
        "include"
        "model"

    REQUIRES
        esp-tflite-micro
        ml_preprocessing

)