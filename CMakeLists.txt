cmake_minimum_required(VERSION 3.0)
project(PROJECT)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-std=c++1y -Wall -Ofast")

message(${CMAKE_SOURCE_DIR})

include_directories(include)

set(SOURCES
)

set(TESTS
)

add_executable(main examples/main.cpp ${SOURCES})
# add_executable(ytest ${SOURCES} ${TESTS})
