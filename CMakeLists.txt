# CMakeList.txt

# Specify the minimum version of CMake required.
# Helps ensure compatibility with features used below.
cmake_minimum_required(VERSION 3.15)

# Define the project name and specify the language (CXX for C++).
# This will also create variables like ${PROJECT_NAME}.
project(hayal-engine LANGUAGES C CXX)


# --- Project Configuration ---

# Set the C++ standard to use.
# Common values: 11, 14, 17, 20, 23
set(CMAKE_CXX_STANDARD 23)
# Enforce the standard strictly (don't allow fallback to older standards).
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Disable compiler-specific extensions (promotes portability).
set(CMAKE_CXX_EXTENSIONS False)

# Enable exporting compile commands.
# This generates a 'compile_commands.json' file in the build directory,
# which is essential for Language Server Protocol (LSP) tools like clangd
# to understand your project structure and provide accurate diagnostics,
# code completion, etc.
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

else()
  # If the TARGET_OS is not one of the explicitly handled cases
  message(FATAL_ERROR "Error: Unsupported or unhandled TARGET_OS: '${TARGET_OS}'.")
endif()

# Define the executable target.
# Syntax: add_executable(<target_name> [source1] [source2] ...)
add_executable(hayal
    ${SELECTED_SOURCE_FILENAME}
    # Add other source files here, e.g.:
    # src/utils.cpp
    # src/engine.cpp
)

# --- Include Directories (Optional) ---

# Specify directories where the compiler should look for header files (.h, .hpp).
# Use target_include_directories for better encapsulation.
# PUBLIC: Headers needed by users of this target (if it were a library).
# PRIVATE: Headers needed only for compiling this target's sources.
# INTERFACE: Headers needed only by users of this target.
# target_include_directories(hayal PRIVATE
#     ${PROJECT_SOURCE_DIR}/include
#     # Add other include paths here
# )
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/glm EXCLUDE_FROM_ALL)

add_library(glad STATIC external/glad/glad.c)
target_include_directories(glad PUBLIC external/glad)

# --- Linking Libraries (Optional) ---

# Specify libraries that the executable needs to link against.
# Use target_link_libraries.
# PUBLIC/PRIVATE/INTERFACE have similar meanings as above regarding usage requirements.

target_link_libraries(hayal PRIVATE SDL2)
target_link_libraries(hayal PRIVATE glad)
target_link_libraries(hayal PRIVATE glm::glm)

# Example: Link against the pthreads library (if doing multi-threading)
# find_package(Threads REQUIRED) # Find the threads library package
# target_link_libraries(hayal PRIVATE Threads::Threads)

# Example: Link against a library found using find_package (e.g., SDL2)
# find_package(SDL2 REQUIRED)
# target_link_libraries(hayal PRIVATE SDL2::SDL2)

# Example: Link against another target defined in this CMake project (if you add libraries)
# add_library(my_helper_lib src/helper.cpp)
# target_link_libraries(hayal PRIVATE my_helper_lib)


# --- Compiler Flags (Optional) ---

# Add specific compiler 
