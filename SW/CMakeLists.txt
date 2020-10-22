cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS
        "$ENV{CodeLib_PATH}/ESP32"
        "$ENV{CodeLib_PATH}/Generic")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(IDF_Dev_Version "4.1")
if (NOT "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" STREQUAL "${IDF_Dev_Version}")
    message(WARNING "The project was tested with ESP-IDF v${IDF_Dev_Version}, it may not work with v${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}")
endif()

set(SUPPORTED_TARGETS esp32)
project(pax-LampD1)

# Configuration checks

if (NOT CONFIG_FREERTOS_HZ EQUAL 100)
    message(FATAL_ERROR "This project requires a 100 Hz FreeRTOS tick rate.")
endif()

## Configuration checks - partitions

if (NOT CONFIG_PARTITION_TABLE_CUSTOM)
    message(FATAL_ERROR "This project requires the custom partition table from the `partitions.csv` file.")
endif()
if (NOT CONFIG_PARTITION_TABLE_CUSTOM_FILENAME STREQUAL "partitions.csv")
    message(FATAL_ERROR "This project requires the custom partition table from the `partitions.csv` file.")
endif()
if (NOT CONFIG_PARTITION_TABLE_FILENAME STREQUAL "partitions.csv")
    message(FATAL_ERROR "This project requires the custom partition table from the `partitions.csv` file.")
endif()

## Configuration checks - HTTP uri length

if (CONFIG_HTTPD_MAX_URI_LEN LESS 1024)
    message(WARNING "This project requires at least 1024 bytes length for HTTP uri.")
endif()
