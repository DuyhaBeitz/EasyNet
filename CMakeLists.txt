cmake_minimum_required(VERSION 3.20)

project(EasyNet LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(EasyNet ${SOURCES})
target_include_directories(EasyNet PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/shared
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/client
)

add_subdirectory(examples)

# include_directories(${CMAKE_SOURCE_DIR}/include)

# set(BUILD_OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

# file(MAKE_DIRECTORY ${BUILD_OUTPUT_DIR})

# file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# add_executable(${PROJECT_NAME} ${SOURCES})

# include_directories("${CMAKE_SOURCE_DIR}/src") 

# set_target_properties(${PROJECT_NAME} PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIR}
# )