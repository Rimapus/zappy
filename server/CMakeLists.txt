if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.16)
    project(zappy_server LANGUAGES CXX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_link_options(-stdlib=libc++)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(
        -Werror
        -O3
        -march=native
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(
        -O0
    )
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

add_executable(zappy_server ${SRC_FILES})

target_compile_options(zappy_server PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -stdlib=libc++
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT error)
    if(ipo_supported)
        set_target_properties(zappy_server PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(STATUS "LTO not supported: ${error}")
    endif()
endif()

target_include_directories(zappy_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    message(STATUS "Updating compile_commands.json symlink to: ${CMAKE_BINARY_DIR}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

find_program(CLANG_TIDY_CACHE_PATH NAMES "clang-tidy-cache")

if(CLANG_TIDY_CACHE_PATH)
    find_program(REAL_CLANG_TIDY NAMES "clang-tidy-20" "clang-tidy")
    set(RESOLVED_CTCACHE_DIR "${CMAKE_BINARY_DIR}/.ctcache")

    add_custom_target(lint
        COMMAND ${CMAKE_COMMAND} -E env
                CTCACHE_DIR=${RESOLVED_CTCACHE_DIR}
                CTCACHE_LOG_LEVEL=info
                sh -c "find ${CMAKE_CURRENT_SOURCE_DIR} -type f \\( -name '*.cpp' -o -name '*.hpp' \\) -not -path '${CMAKE_CURRENT_SOURCE_DIR}/build/*' | xargs -P0 -n 1 ${CLANG_TIDY_CACHE_PATH} ${REAL_CLANG_TIDY} -p ${CMAKE_BINARY_DIR}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running parallel precise clang-tidy file scan accelerated by ctcache..."
        VERBATIM
    )
else()
    find_program(REAL_CLANG_TIDY NAMES "clang-tidy-20" "clang-tidy")
    if(REAL_CLANG_TIDY)
        add_custom_target(lint
            COMMAND sh -c "find ${CMAKE_CURRENT_SOURCE_DIR} -type f \\( -name '*.cpp' -o -name '*.hpp' \\) -not -path '${CMAKE_CURRENT_SOURCE_DIR}/build/*' | xargs -P0 -n 1 ${REAL_CLANG_TIDY} -p ${CMAKE_BINARY_DIR}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Running parallel precise clang-tidy file scan..."
            VERBATIM
        )
    else()
        message(WARNING "Neither ctcache nor clang-tidy could be configured for the lint target.")
    endif()
endif()