cmake_minimum_required(VERSION 3.22.1)
project(native_whisper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_library(
    native_whisper
    SHARED
    native_whisper.cpp
    whisper.cpp
    ggml/ggml.c
    ggml/ggml-alloc.c
    ggml/ggml-quants.c
    ggml/ggml-cpu.cpp
    ggml/ggml-cpu-traits.cpp
    ggml/ggml-threading.cpp
    ggml/ggml-backend.cpp
    ggml/ggml-backend-reg.cpp
    ggml/ggml-cpu-stubs.cpp
)


target_include_directories(native_whisper PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/ggml
)

find_library(log-lib log)
find_library(android-lib android)

target_link_libraries(native_whisper
    ${log-lib}
    ${android-lib}
    atomic
    m
)

