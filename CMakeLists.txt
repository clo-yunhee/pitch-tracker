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
    src/sdl2/audio.cpp
    src/sdl2/audio.h
    src/tracker/tracker.h
    src/tracker/context.cpp
    src/tracker/context.h
    src/tracker/analysis.cpp
    src/tracker/processors/processors.h
    src/tracker/processors/filter_fir.cpp
    src/tracker/processors/filter_iir.cpp
    src/tracker/processors/lpc.cpp
    src/tracker/processors/lpc_residual.cpp
    src/tracker/processors/hanning.cpp
    src/tracker/processors/pitch_acf.cpp
    src/app/app.h
    src/app/context.cpp
    src/app/context.h
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)
find_package(FFTW REQUIRED DOUBLE_LIB)

if(NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 not found")
endif()

if(NOT Eigen3_FOUND)
    message(FATAL_ERROR "Eigen3 not found")
endif()

include_directories(${SDL2_INCLUDE_DIRS})
link_libraries(Threads::Threads ${SDL2_LIBRARIES} Eigen3::Eigen ${FFTW_LIBRARIES}) 

add_executable(pitch-tracker ${SOURCES})
