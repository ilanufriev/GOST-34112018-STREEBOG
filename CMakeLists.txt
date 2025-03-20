# Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
# SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

cmake_minimum_required(VERSION 3.20)

project(libgost34112018 VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(TARGET_TEST test_gost34112018)
set(TARGET_LIB  gost34112018)
set(TARGET_UTIL gost34112018_cli)

set(TARGET_LIB_COMMON_FILES
        src/lib/gost34112018_vec512.c
        src/lib/gost34112018_common.c
        src/lib/gost34112018.c
    )

set(TARGET_LIB_COMMON_INCLUDE_DIRS
        include
        src
        src/lib
    )

if(NOT LIBGOST34112018_TYPE)
    set(LIBGOST34112018_TYPE OPTIMIZED)
endif()

# for test purposes
add_executable(${TARGET_TEST} src/test/test.c)

# util for copmuting STREEBOG hash of various data from cli
add_executable(${TARGET_UTIL} src/util/gost34112018_cli.c)

if(LIBGOST34112018_TYPE STREQUAL "OPTIMIZED")
    message("Chosen OPTIMIZED implementation.")

    add_library(${TARGET_LIB} SHARED
            ${TARGET_LIB_COMMON_FILES}
            src/lib/optimized/gost34112018_optimized.c
        )

    target_include_directories(${TARGET_LIB} PUBLIC
            ${TARGET_LIB_COMMON_INCLUDE_DIRS}
            src/lib/optimized
        )
elseif(LIBGOST34112018_TYPE STREQUAL "REFERENCE")
    message("Chosen REFERENCE implementation.")

    add_library(${TARGET_LIB} SHARED
            ${TARGET_LIB_COMMON_FILES}
            src/lib/reference/gost34112018_ref.c
        )

    target_include_directories(${TARGET_LIB} PUBLIC
            ${TARGET_LIB_COMMON_INCLUDE_DIRS}
            src/lib/reference
        )
else()
    message(FATAL_ERROR "No library type given.")
endif()

target_include_directories(${TARGET_TEST} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${TARGET_UTIL} PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${TARGET_TEST} PUBLIC ${TARGET_LIB})
target_link_libraries(${TARGET_UTIL} PUBLIC ${TARGET_LIB})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Using debug compile options")
    target_compile_options(${TARGET_LIB} PUBLIC
            -Wall
            -Wextra
            -Wpedantic
            -fsanitize=address
            -fsanitize=undefined
        )

    target_link_options(${TARGET_LIB} PUBLIC
            -fsanitize=address
            -fsanitize=undefined
        )

        target_compile_definitions(${TARGET_LIB} PUBLIC DEBUG)
        target_compile_definitions(${TARGET_TEST} PUBLIC DEBUG)
        target_compile_definitions(${TARGET_UTIL} PUBLIC DEBUG)
endif()