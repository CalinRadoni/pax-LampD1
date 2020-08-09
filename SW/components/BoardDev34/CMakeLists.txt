set(c_SOURCE_FILES
    "BoardDev34.cpp"
    "HTTPC2Server.cpp"
)

set(c_PRIVATE_REQUIREMENTS
    BoardManager
    ESP32HAL
    Debouncer
    ESP32SimpleOTA
    esp_http_server
    json
)

set(c_EMBED_FILES
    "index.html"
)

idf_component_register(
    SRCS ${c_SOURCE_FILES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES ${c_PRIVATE_REQUIREMENTS}
    EMBED_FILES ${c_EMBED_FILES}
)
