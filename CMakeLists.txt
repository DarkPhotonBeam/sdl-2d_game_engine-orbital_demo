cmake_minimum_required(VERSION 3.31)
project(sdl_2d_engine-orbital_demo C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/2DEngine EXCLUDE_FROM_ALL)

add_executable(sdl_2d_engine-orbital_demo src/main.c)

target_link_libraries(sdl_2d_engine-orbital_demo PRIVATE SDL3::SDL3 DLeuthold::2DEngine m)