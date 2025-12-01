cmake_minimum_required(VERSION 3.10)

project(LogSearchProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(COMMON_SOURCES
    ./index_manager.cpp
)

add_executable(indexer
    ./indexer.cpp
    ${COMMON_SOURCES}
)

add_executable(searcher 
    ./searcher.cpp 
    ${COMMON_SOURCES}
)