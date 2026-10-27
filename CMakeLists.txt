cmake_minimum_required(VERSION 3.16)
project(zappy LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_subdirectory(server)
add_subdirectory(ai)
add_subdirectory(gui)
