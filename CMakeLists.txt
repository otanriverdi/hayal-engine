cmake_minimum_required(VERSION 3.15)
project(hayal-engine 
    VERSION 0.1.0
    DESCRIPTION "Hayal Engine - An experimental game engine"
    HOMEPAGE_URL "https://github.com/otanriver/hayal-engine"
    LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# --- Project Configuration ---

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Build Targets ---

if(NOT TARGET_OS)
    set(TARGET_OS "${CMAKE_SYSTEM_NAME}")
    message(STATUS "TARGET_OS not specified, defaulting to current system: ${TARGET_OS}")
else()
    message(STATUS "TARGET_OS specified: ${TARGET_OS}")
endif()

if("${TARGET_OS}" STREQUAL "Linux")
    set(SELECTED_SOURCE_FILENAME "src/main_linux.cpp")
    message(STATUS "Detected Linux target, expecting source file: ${SELECTED_SOURCE_FILENAME}")
elseif("${TARGET_OS}" STREQUAL "Darwin")
    set(SELECTED_SOURCE_FILENAME "src/main_darwin.cpp")
    message(STATUS "Detected Darwin target, expecting source file: ${SELECTED_SOURCE_FILENAME}")
else()
    message(FATAL_ERROR "Error: Unsupported or unhandled TARGET_OS: '${TARGET_OS}'.")
endif()

add_executable(hayal
    ${SELECTED_SOURCE_FILENAME}
    # Add other source files here, e.g.:
    # src/utils.cpp
    # src/engine.cpp
)

# --- Include Directories (Optional) ---

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_include_directories(hayal PRIVATE external/imgui)
    target_include_directories(hayal PRIVATE external/imgui/backends)

    file(GLOB DEBUG_SOURCES
        "external/imgui/*.cpp"
        "external/imgui/*.h"
        "external/imgui/backends/imgui_impl_opengl3.h"
        "external/imgui/backends/imgui_impl_opengl3.cpp"
        "external/imgui/backends/imgui_impl_sdl2.h"
        "external/imgui/backends/imgui_impl_sdl2.cpp"
    )
    target_sources(hayal PRIVATE ${DEBUG_SOURCES})
    target_compile_definitions(hayal PRIVATE DEBUG)
endif()

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/glm EXCLUDE_FROM_ALL)

add_library(glad STATIC external/glad/glad.c)
target_include_directories(glad PUBLIC external/glad)

target_link_libraries(hayal PRIVATE SDL2)
target_link_libraries(hayal PRIVATE glad)
target_link_libraries(hayal PRIVATE glm::glm)

target_compile_options(hayal PRIVATE
    $<$<CONFIG:Debug>:
        -O0
        -g
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wformat=2
        -Wcast-align
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wold-style-cast
        -D_DEBUG
    >
    $<$<CONFIG:Release>:
        -O3
        -DNDEBUG
        -march=native
    >
)
