cmake_minimum_required(VERSION 3.20)

project(EasyNet LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

option(EASYNET_BUILD_EXAMPLES "Build examples" OFF)

add_library(${PROJECT_NAME} STATIC 
    src/EasyNetLog.cpp
    src/EasyNetClient.cpp
    src/EasyNetServer.cpp
    src/EasyNetShared.cpp
)

target_include_directories(EasyNet
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

find_package(fmt QUIET)
if (NOT fmt_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 11.0.2
    )
    FetchContent_MakeAvailable(fmt)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

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

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Config.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)