cmake_minimum_required(VERSION 3.10)

project(LogSearchProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(PROJECT_SOURCES
    ./log_search.cpp
    ./index_manager.cpp
)

add_executable(log_search ${PROJECT_SOURCES})