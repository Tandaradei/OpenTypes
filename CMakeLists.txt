cmake_minimum_required(VERSION 3.0)

project(OpenTypes)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE external/catch.hpp)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/open_types.hpp
    src/my_types.hpp
)

add_executable(${PROJECT_NAME}_tests
    test/test_types.hpp
    test/test.cpp
)

target_link_libraries(${PROJECT_NAME}_tests Catch)

enable_testing()
add_test(NAME MyAwesomeTest COMMAND ${PROJECT_NAME}_tests)
