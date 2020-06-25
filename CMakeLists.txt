cmake_minimum_required(VERSION 3.5)
project(pitch-tracker)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_support")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fPIC -ftree-vectorize")
set(CMAKE_CXX_FLAGS_DEBUG           "-O0 -g")
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
    src/sdl2/font.cpp
    src/sdl2/font.h
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
    src/tracker/processors/pitch_amdf_m.cpp
    src/tracker/processors/pick_peaks.cpp
    src/app/app.h
    src/app/context.cpp
    src/app/context.h
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2 SDL2_ttf SDL2_gfx)
pkg_check_modules(Eigen3 REQUIRED eigen3)
pkg_check_modules(FFTW REQUIRED fftw3)

include_directories(${SDL2_INCLUDE_DIRS} ${Eigen3_INCLUDE_DIRS} ${FFTW_INCLUDE_DIRS})
link_libraries(Threads::Threads ${SDL2_LDFLAGS} ${Eigen3_LDFLAGS} ${FFTW_LDFLAGS})

if(ANDROID)
    add_library(pitch-tracker SHARED ${SOURCES})
else()
    add_executable(pitch-tracker ${SOURCES})
endif()
