# SPDX-License-Identifier: Apache-2.0

add_custom_target(run_python_script
    COMMAND python3 sprite_factory.py
    # Sets the execution context to your subfolder
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rsc"
    VERBATIM
)

cmake_minimum_required(VERSION 3.28.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(tetrimini)

set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)
generate_inc_file_for_target(app rsc/bapple.bin ${gen_dir}/bapple.bin.inc)

target_include_directories(app PRIVATE src/inc)

target_sources(app PRIVATE
  src/main.c
  src/graphics.c
  src/playfield.c
  src/sprites.c
  src/tetromino.c
  src/mediaplayer.c
)

add_dependencies(app run_python_script)