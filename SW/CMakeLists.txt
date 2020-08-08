cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS
        "$ENV{CodeLib_PATH}/ESP32"
        "$ENV{CodeLib_PATH}/Generic")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(pax-LampD1)
