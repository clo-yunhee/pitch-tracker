cmake_minimum_required(VERSION 3.5)
project(pitch-tracker)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_support")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fPIC -ftree-vectorize")
set(CMAKE_CXX_FLAGS_DEBUG           "-00 -g")
set(CMAKE_CXX_FLAGS_RELEASE         "-O3 -g0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL      "-Os -g0")

set(SOURCES
    src/main.cpp
    src/sdl2/sdl2.h
    src/sdl2/context.cpp
    src/sdl2/context.h
)

find_package(SDL2 REQUIRED)
find_package(FFTW REQUIRED DOUBLE_LIB)

if(NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 not found")
endif()

include_directories(${SDL2_INCLUDE_DIRS})
link_libraries(${SDL2_LIBRARIES})

add_executable(pitch-tracker ${SOURCES})
