cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0079 NEW)
set(FAMILY rp2040)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_EXTRAS_PATH}/external/pico_extras_import.cmake)

set(PROJECT hello_world)
project(${PROJECT})
pico_sdk_init()

add_compile_options(-Wall)
add_executable(${PROJECT})

include_directories(
	$ENV{PICO_DVI_HOME}/include
)

target_sources(${PROJECT} PUBLIC
    src/main.cpp
    src/data.cpp
    src/game.cpp
    src/map.cpp
    src/maparch.cpp
    src/tilesdata.cpp
    src/actor.cpp
    src/animator.cpp
    src/engine.cpp
    src/draft.cpp
)

# Example include
target_include_directories(${PROJECT} PUBLIC
    src
)

target_link_libraries(${PROJECT} PUBLIC 
    pico_stdlib
    pico_multicore
    pico_unique_id
    hardware_i2c
    libgvga
)


target_compile_definitions(${PROJECT} PRIVATE
PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS=700
)

add_subdirectory($ENV{GVGA_HOME}/libs ${CMAKE_BINARY_DIR}/libgvga)
set_property(SOURCE src/data.s APPEND PROPERTY COMPILE_OPTIONS "-I" "${CMAKE_SOURCE_DIR}/data")
set_property(TARGET ${PROJECT} APPEND_STRING PROPERTY LINK_FLAGS "-Wl,--print-memory-usage")


target_link_libraries(${PROJECT} PRIVATE
        pico_multicore
        pico_stdlib
        pico_scanvideo_dpi
        libgvga
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT})
