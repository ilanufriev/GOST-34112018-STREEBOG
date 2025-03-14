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
add_library(${TARGET_LIB} SHARED src/gost34112018.c)

target_include_directories(${TARGET_LIB} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${TARGET_LIB} PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_include_directories(${TARGET_TEST} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${TARGET_TEST} PUBLIC ${TARGET_LIB})

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(${TARGET_LIB} PUBLIC DEBUG)
    target_compile_definitions(${TARGET_TEST} PUBLIC DEBUG)
endif()