# Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
# SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

cmake_minimum_required(VERSION 3.20)

project(libgost34112018 VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(TARGET_TEST test_gost34112018)
set(TARGET_LIB gost34112018)

# for test purposes
add_executable(${TARGET_TEST} src/test/test.c)

# main library
add_library(${TARGET_LIB} SHARED src/lib/gost34112018_optimized.c)

target_include_directories(${TARGET_LIB} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${TARGET_LIB} PUBLIC ${CMAKE_SOURCE_DIR}/src/lib)

target_include_directories(${TARGET_TEST} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${TARGET_TEST} PUBLIC ${TARGET_LIB})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Using debug compile options")
    target_compile_options(${TARGET_LIB} PUBLIC -Wall
                                                -Wextra
                                                -Wpedantic
                                                -fsanitize=address
                                                -fsanitize=undefined
                                                 )

    target_link_options(${TARGET_LIB} PUBLIC -fsanitize=address
                                             -fsanitize=undefined
                                              )

    target_compile_definitions(${TARGET_LIB} PUBLIC DEBUG)
    target_compile_definitions(${TARGET_TEST} PUBLIC DEBUG)
endif()