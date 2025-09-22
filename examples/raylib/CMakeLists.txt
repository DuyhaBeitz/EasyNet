
add_executable(server_raylib server_raylib.cpp)
target_link_libraries(server_raylib PRIVATE EasyNet)

add_executable(client_raylib client_raylib.cpp)
find_package(raylib)
target_link_libraries(client_raylib PRIVATE
    EasyNet
    raylib
)

#target_include_directories(client_raylib PRIVATE ${raylib_INCLUDE_DIR})