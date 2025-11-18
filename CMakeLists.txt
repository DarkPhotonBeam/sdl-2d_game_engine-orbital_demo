cmake_minimum_required(VERSION 3.31)
project(sdl_velocity_verlet C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(sdl_velocity_verlet main.c
        vector2d.c
        vector2d.h
        rigidbody2d.c
        rigidbody2d.h
        game.c
        game.h
)

target_link_libraries(sdl_velocity_verlet PRIVATE SDL3::SDL3 m)