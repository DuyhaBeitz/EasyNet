cmake_minimum_required(VERSION 3.20)

project(EasyNet LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)

option(EASYNET_BUILD_EXAMPLES "Build examples" OFF)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(EasyNet ${SOURCES})
target_include_directories(EasyNet PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/shared
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/client
)

if(EASYNET_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()