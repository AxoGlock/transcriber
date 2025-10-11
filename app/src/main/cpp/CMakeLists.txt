cmake_minimum_required(VERSION 3.22.1)
project(native_whisper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/ggml
)

# GGML source files (latest structure)
set(GGML_SRC
    ${CMAKE_SOURCE_DIR}/ggml/ggml.c
    ${CMAKE_SOURCE_DIR}/ggml/ggml-alloc.c
    ${CMAKE_SOURCE_DIR}/ggml/ggml-backend.cpp
    ${CMAKE_SOURCE_DIR}/ggml/ggml-backend-reg.cpp
    ${CMAKE_SOURCE_DIR}/ggml/ggml-cpu.cpp
    ${CMAKE_SOURCE_DIR}/ggml/ggml-quants.c
)

add_library(ggml STATIC ${GGML_SRC})

# Native Whisper source files
set(WHISPER_SRC
    ${CMAKE_SOURCE_DIR}/whisper.cpp
    ${CMAKE_SOURCE_DIR}/native_whisper.cpp
)

add_library(native_whisper SHARED ${WHISPER_SRC})

# Link libraries
target_link_libraries(
    native_whisper
    ggml
    log
    android
    atomic
    m
)
