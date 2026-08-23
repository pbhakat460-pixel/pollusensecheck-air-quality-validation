idf_component_register(

    SRCS
        "ml_manager.c"

    INCLUDE_DIRS
        "include"

    REQUIRES

        feature_builder
        ml_preprocessing
        tinyml

)