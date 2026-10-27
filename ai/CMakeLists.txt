if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.16)
    project(zappy_ai LANGUAGES CXX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE AI_SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(zappy_ai ${AI_SOURCES})

target_compile_options(zappy_ai PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(zappy_ai PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
