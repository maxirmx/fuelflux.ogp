cmake_minimum_required(VERSION 3.10)
project(fuelflux_ogp C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags (equivalent to Makefile's -O2 -Wall -Wextra)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-O2")

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
