cmake_minimum_required(VERSION 3.20)

project(EasyNet LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)

option(EASYNET_BUILD_EXAMPLES "Build examples" OFF)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>

        $<INSTALL_INTERFACE:include>
)

if(EASYNET_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()


################################### Installation
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install public headers (everything in src/ that looks like a header)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.hpp"
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/enet/enet.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/enet/
)

# Generate a config file for find_package()
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Config.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)