cmake_minimum_required(VERSION 3.0)

project(OpenTypes)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/)

SET(BASEPATH "${CMAKE_SOURCE_DIR}")
INCLUDE_DIRECTORIES("${BASEPATH}")

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE external/catch.hpp)
add_library(cereal INTERFACE)
target_include_directories(cereal INTERFACE cereal/)

add_executable(${PROJECT_NAME}_${OUTPUT_NAME}
    src/main.cpp
    src/open_types.hpp
    src/Types/Car.hpp
    src/Types/Skill.hpp
    src/Types/Person.hpp
    src/Types/List.hpp
    src/Helper.hpp
)

target_link_libraries(${PROJECT_NAME}_${OUTPUT_NAME} cereal)

add_executable(${PROJECT_NAME}_${OUTPUT_NAME}_tests
    test/Types/Skill.hpp
    test/Types/Person.hpp
    test/Types/Car.hpp
    test/main.cpp
    test/Types.test.cpp
    test/Attributes.test.cpp
    test/Relations.test.cpp
)

target_link_libraries(${PROJECT_NAME}_${OUTPUT_NAME}_tests cereal Catch)

enable_testing()
add_test(NAME MyAwesomeTest COMMAND ${PROJECT_NAME}_tests)
