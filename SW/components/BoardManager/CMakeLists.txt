set(c_SOURCE_FILES
    "Board.cpp"
    "Configuration.cpp"
    "Events.cpp"
    "HTTPConfigServer.cpp"
    "WiFiManager.cpp"
    "WiFiConfig.cpp"
)

set(c_PRIVATE_REQUIREMENTS
    nvs_flash
    esp_http_server
)

set(c_EMBED_FILES
    "index.html"
    "jquery.js"
)

idf_component_register(
    SRCS ${c_SOURCE_FILES}
    INCLUDE_DIRS "."
    REQUIRES ESP32HAL
    PRIV_REQUIRES ${c_PRIVATE_REQUIREMENTS}
    EMBED_FILES ${c_EMBED_FILES}
)
