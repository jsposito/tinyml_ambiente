idf_component_register(
    SRCS "ambiente_sensores.cpp"

    PRIV_REQUIRES
        esp_adc
        esp_driver_gpio

    INCLUDE_DIRS "."

    EMBED_FILES
        "modelo_ambiente_int8.tflite"
)