cmake_minimum_required(VERSION 3.18)
project(native_whisper)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ggml)

# Add library
add_library(native_whisper SHARED
        native_whisper.cpp
        whisper.cpp

        # ggml core
        ggml/ggml.cpp
        ggml/ggml-cpu.cpp
        ggml/ggml-backend.cpp
        ggml/ggml-backend-reg.cpp
        ggml/ggml-threading.cpp
        ggml/ggml-opt.cpp
        ggml/gguf.cpp
        ggml/ggml.h
        ggml/ggml.c
        ggml/ggml-quants.c
        ggml/ggml-cpu.cpp
        ggml-cpu-impl.cpp
        ggml-alloc.cpp
        # ggml ops
        ggml/binary-ops.cpp
        ggml/unary-ops.cpp
        ggml/ops.cpp
        ggml/vec.cpp
        ggml/repack.cpp
        ggml/traits.cpp
        ggml/hbm.cpp
)

# Compile definitions
target_compile_definitions(native_whisper PRIVATE GGML_NO_AMX)

# Link system libraries
find_library(log-lib log)
find_library(android-lib android)
find_library(m-lib m)
find_library(atomic-lib atomic)  # may fail on some ABIs

# Only link atomic if found
if(atomic-lib)
    target_link_libraries(native_whisper
            ${log-lib}
            ${android-lib}
            ${m-lib}
            ${atomic-lib}
    )
else()
    target_link_libraries(native_whisper
            ${log-lib}
            ${android-lib}
            ${m-lib}
    )
endif()
