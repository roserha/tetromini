cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Tetromini)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM, C++ and C languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Generate sprite data before building the firmware
add_custom_target(make_sprites
    COMMAND python3 "${CMAKE_SOURCE_DIR}/Scripts/sprite_factory.py"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Scripts"
    VERBATIM
)

add_dependencies(${CMAKE_PROJECT_NAME} make_sprites)

# Generate melody data before building the firmware
add_custom_target(make_melodies
    COMMAND python3 "${CMAKE_SOURCE_DIR}/Scripts/song_factory.py"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Scripts"
    VERBATIM
)

add_dependencies(${CMAKE_PROJECT_NAME} make_melodies)

# Add bapple easter egg video to binary by generating object file from bapple.bin
# We won't autogenerate it on every build bc the bapple tree script takes an ETERNITY to execute
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bapple.o
    COMMAND arm-none-eabi-objcopy --input-target binary --output-target elf32-littlearm 
            --rename-section .data=.rodata,alloc,load,readonly,data,contents
            --binary-architecture arm bapple.bin 
            ${CMAKE_CURRENT_BINARY_DIR}/bapple.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Scripts
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/bapple.bin
    VERBATIM
)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    App/app.cpp
    App/Drivers/ssd1309.cpp
    App/Drivers/zlib_turbo.cpp
    App/Controllers/graphics.cpp
    App/Controllers/sprites.cpp
    App/Controllers/tetromino.cpp
    App/Controllers/playfield.cpp
    App/Controllers/audio.cpp
    App/Controllers/melody.cpp
    App/Controllers/video.cpp
    App/Controllers/decoder.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/bapple.o
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    App
    App/Drivers/Inc
    App/Controllers/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
