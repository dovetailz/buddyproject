cmake_minimum_required(VERSION 3.10)

# Project name
project(BUDDYSERVER VERSION 1.0)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_subdirectory(Server)
add_subdirectory(Application)

configure_file(.clang-format .clang-format)
add_custom_target(format
    COMMAND ../format.sh)