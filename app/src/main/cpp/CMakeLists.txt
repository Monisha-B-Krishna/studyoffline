cmake_minimum_required(VERSION 3.22.1)
project(studyoffline)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(llama.cpp)

add_library(studyoffline_native SHARED
    native-lib.cpp
)

target_link_libraries(studyoffline_native
    llama
    common
    android
    log
)