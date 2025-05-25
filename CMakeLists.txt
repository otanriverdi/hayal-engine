cmake_minimum_required(VERSION 3.15)

project(hayal-engine 
    VERSION 0.1.0
    DESCRIPTION "Hayal Engine - An experimental game engine"
    HOMEPAGE_URL "https://github.com/otanriver/hayal-engine"
    LANGUAGES C CXX
)

# -------------------- Build Type --------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# -------------------- Compiler Configuration --------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure we're using Clang with libc++
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Clang detected: Enabling libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
endif()

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -------------------- OS-Specific Entry Point --------------------
if(NOT TARGET_OS)
    set(TARGET_OS "${CMAKE_SYSTEM_NAME}")
    message(STATUS "TARGET_OS not specified, defaulting to current system: ${TARGET_OS}")
else()
    message(STATUS "TARGET_OS specified: ${TARGET_OS}")
endif()

if("${TARGET_OS}" STREQUAL "Linux")
    set(SELECTED_SOURCE_FILENAME "src/main_linux.cpp")
else()
    message(FATAL_ERROR "Error: Unsupported or unhandled TARGET_OS: '${TARGET_OS}'.")
endif()

# -------------------- Main Executable --------------------
add_executable(hayal
    ${SELECTED_SOURCE_FILENAME}
)

# -------------------- External Libraries --------------------

# SDL2
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
target_link_libraries(hayal PRIVATE SDL2)

# GLAD
add_library(glad STATIC external/glad/glad.c)
target_include_directories(glad PUBLIC external/glad)
target_link_libraries(hayal PRIVATE glad)

# stb_image
add_library(stb_image STATIC external/stb_image/stb_image.c)
target_include_directories(stb_image PUBLIC external/stb_image)
target_link_libraries(hayal PRIVATE stb_image)

# miniaudio
add_library(miniaudio STATIC external/miniaudio/miniaudio.c)
target_include_directories(miniaudio PUBLIC external/miniaudio)
target_link_libraries(hayal PRIVATE miniaudio)

# glm - header-only (no add_subdirectory!)
target_include_directories(hayal PRIVATE external/glm)

# -------------------- Compile Options --------------------
target_compile_options(hayal PRIVATE
    $<$<CONFIG:Debug>:
        -O0 -g
        -Wall -Wextra -Wpedantic
        -Wshadow -Wformat=2 -Wcast-align
        -Wconversion -Wsign-conversion
        -Wnull-dereference -Wdouble-promotion
        -Wold-style-cast -D_DEBUG
    >
    $<$<CONFIG:Release>:
        -O3 -DNDEBUG -march=native
    >
)

# -------------------- Link Options --------------------
target_link_options(hayal PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>
)
