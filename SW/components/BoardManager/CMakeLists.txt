set(c_SOURCE_FILES
    "Board.cpp"
    "Configuration.cpp"
    "Events.cpp"
    "pax_http_server.cpp"
    "WiFiManager.cpp"
    "WiFiConfig.cpp"
)

set(c_PRIVATE_REQUIREMENTS
    nvs_flash
)

set(c_REQUIREMENTS
    esp_http_server
    json
    ESP32HAL
    ESP32SimpleOTA
)

set(c_EMBED_FILES
    "${CMAKE_SOURCE_DIR}/web/index.html"
)

idf_component_register(
    SRCS ${c_SOURCE_FILES}
    INCLUDE_DIRS "."
    REQUIRES ${c_REQUIREMENTS}
    PRIV_REQUIRES ${c_PRIVATE_REQUIREMENTS}
    EMBED_FILES ${c_EMBED_FILES}
)
