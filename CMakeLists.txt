cmake_minimum_required(VERSION 3.0)
project(PROJECT)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-std=c++1y -Wall -Ofast -lsfml-system -lsfml-window -lsfml-graphics")

message(${CMAKE_SOURCE_DIR})

include_directories(include)

set(SOURCES
src/component.hpp
src/entity.hpp
src/helpers.hpp
src/tmp.hpp
src/traits.hpp
)

set(TESTS
)

# add_executable(main examples/main.cpp ${SOURCES})
add_executable(sfml examples/sfml.cpp ${SOURCES})
# add_executable(ytest ${SOURCES} ${TESTS})
