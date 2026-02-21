cmake_minimum_required(VERSION 3.10)
project(fuelflux_ogp C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags (equivalent to Makefile's -O2 -Wall -Wextra)
add_compile_options(-Wall -Wextra)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Match Makefile's -O2 optimization level for Release builds
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "Release flags" FORCE)
set(CMAKE_C_FLAGS_DEBUG "-g" CACHE STRING "Debug flags" FORCE)

# Find libgpiod
find_library(GPIOD_LIBRARY gpiod)

if(NOT GPIOD_LIBRARY)
    message(WARNING "libgpiod not found. Install libgpiod-dev to build.")
    message(WARNING "On Debian/Ubuntu: sudo apt-get install libgpiod-dev")
    # Set a placeholder to allow configuration to complete
    set(GPIOD_LIBRARY "gpiod")
endif()

# Add executable
add_executable(pulse_test pulse_test.c)

# Link libraries
target_link_libraries(pulse_test ${GPIOD_LIBRARY})

# Installation (optional)
install(TARGETS pulse_test DESTINATION bin)
