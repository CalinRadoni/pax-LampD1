set(c_SOURCE_FILES
    "main.cpp"
)

set(c_PRIVATE_REQUIREMENTS
    BoardLampD1
    ESP32BoardManager
    Debouncer
    ESP32SimpleOTA
    ESP32DLEDController
    esp_http_server
)

idf_component_register(
    SRCS ${c_SOURCE_FILES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES ${c_PRIVATE_REQUIREMENTS}
)
