find_path(CRITERION_INCLUDE_DIRS criterion/criterion.h
    PATHS /usr/local/include /opt/homebrew/include /usr/include
)
find_library(CRITERION_LIBRARIES criterion
    PATHS /usr/local/lib /opt/homebrew/lib /usr/lib
)
if(NOT CRITERION_INCLUDE_DIRS OR NOT CRITERION_LIBRARIES)
    message(FATAL_ERROR "Criterion not found. Install it or set BUILD_TESTS=OFF.")
endif()

file(GLOB_RECURSE LIB_SRC CONFIGURE_DEPENDS ../src/*.cpp)
list(FILTER LIB_SRC EXCLUDE REGEX ".*/Main\\.cpp$")

file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS *.cpp)

add_executable(zappy_tests ${LIB_SRC} ${TEST_SRC})

target_compile_options(zappy_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -stdlib=libc++
)

target_include_directories(zappy_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CRITERION_INCLUDE_DIRS}
)

target_link_options(zappy_tests PRIVATE -stdlib=libc++)
target_link_libraries(zappy_tests PRIVATE ${CRITERION_LIBRARIES})

option(BUILD_COVERAGE "Build with code coverage instrumentation" OFF)

if(BUILD_COVERAGE)
    target_compile_options(zappy_tests PRIVATE --coverage)
    target_link_options(zappy_tests PRIVATE --coverage)

    find_program(GCOVR NAMES gcovr)
    if(NOT GCOVR)
        message(FATAL_ERROR "gcovr not found. Install it or set BUILD_COVERAGE=OFF.")
    endif()

    find_program(LLVM_COV NAMES llvm-cov-20 llvm-cov-19 llvm-cov-18 llvm-cov)
    if(LLVM_COV)
        set(GCOV_WRAPPER "${CMAKE_BINARY_DIR}/gcov_wrapper.sh")
        file(WRITE "${GCOV_WRAPPER}" "#!/bin/sh\nexec \"${LLVM_COV}\" gcov \"$@\"\n")
        execute_process(COMMAND chmod +x "${GCOV_WRAPPER}")
        set(GCOVR_GCOV_OPT "--gcov-executable=${GCOV_WRAPPER}")
    else()
        set(GCOVR_GCOV_OPT "")
    endif()

    set(COVERAGE_FILES
        ${CMAKE_SOURCE_DIR}/coverage.html
        ${CMAKE_SOURCE_DIR}/coverage.css
        ${CMAKE_SOURCE_DIR}/coverage.*.html
    )

    add_custom_target(coverage-clean
        COMMAND rm -f ${COVERAGE_FILES}
        COMMAND find ${CMAKE_BINARY_DIR} -name "*.gcda" -delete
        COMMENT "Removing coverage reports and instrumentation data"
    )

    add_custom_target(coverage
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/zappy_tests
        COMMAND ${GCOVR} ${GCOVR_GCOV_OPT} --html-details ${CMAKE_SOURCE_DIR}/coverage.html --exclude 'tests/.*'
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS coverage-clean zappy_tests
    )
endif()
