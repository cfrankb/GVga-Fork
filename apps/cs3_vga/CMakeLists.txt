cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0079 NEW)
set(FAMILY rp2040)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_EXTRAS_PATH}/external/pico_extras_import.cmake)

#execute_process(COMMAND git describe --tags --dirty --always OUTPUT_VARIABLE GIT_DESCRIBE OUTPUT_STRIP_TRAILING_WHITESPACE)

set(PROJECT rp2040_gvga_cs3)
project(${PROJECT})
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(COMMON_COMPILE_OPTIONS
  -Wall -Wextra -Wno-unused-parameter -Wstack-usage=800 # some enable extra warnings
  -fasynchronous-unwind-tables        # backtrace support # -fno-omit-frame-pointer
)
#set(PICO_BOARD pico_w)                # we only support the pico w
#set(PICO_NO_GC_SECTIONS 1)            # don't remove unused functions at link time (so a custom prog can call them)


pico_sdk_init()

add_compile_options(-Wall)
add_executable(${PROJECT})

include_directories(
	$ENV{PICO_DVI_HOME}/include
)

target_sources(${PROJECT} PUBLIC
    src/main.cpp
    src/game.cpp
    src/map.cpp
    src/tilesdata.cpp
    src/actor.cpp
    src/animator.cpp
    src/engine.cpp
    src/draft.cpp
    src/joystick.cpp
    src/debug.cpp
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
        hardware_flash
        libgvga
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT})
