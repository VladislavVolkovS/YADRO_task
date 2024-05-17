cmake_minimum_required(VERSION 3.14)
project(YADRO_test)

set(CMAKE_CXX_STANDARD 20)

find_package(Catch2 REQUIRED)
add_executable(YADRO_test test.cpp club.cpp)
target_link_libraries(YADRO_test Catch2::Catch2)

add_executable(YADRO_run main.cpp club.cpp)

include(Catch)
catch_discover_tests(YADRO_test)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
