cmake_minimum_required(VERSION 3.10)

project(native_whisper)

# You may want to change this list if you add/remove files.
set(SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(GGML_DIR "${SRC_ROOT}/ggml")
set(ggml-cpu_DIR "${SRC_ROOT}/ggml/ggml-cpu")

# Source files
set(SOURCES
    ${SRC_ROOT}/whisper_native.cpp              # JNI wrapper + small stubs
    ${SRC_ROOT}/whisper.cpp                     # main whisper implementation (from upstream)
    ${GGML_DIR}/ggml.c
    ${GGML_DIR}/ggml-alloc.c
    ${GGML_DIR}/ggml-quants.c
    ${GGML_DIR}/ggml-threading.cpp
    ${GGML_DIR}/ggml.cpp
    ${GGML_DIR}/ggml-backend.cpp
    ${GGML_DIR}/ggml-backend-reg.cpp
    ${GGML_DIR}/ggml-opt.cpp
)

# Create library
add_library(native_whisper SHARED ${SOURCES})

# Use C++17 (whisper usually requires C++17)
set_target_properties(native_whisper PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Important: compile definitions to disable non-Android backends.
# WHISPER_NO_COREML disables CoreML references.
# WHISPER_NO_METAL disables Metal.
# WHISPER_NO_VULKAN disables automatic vulkan/ggml tries.
# (If your whisper/ggml header uses different macro names, adjust accordingly.)
target_compile_definitions(native_whisper PRIVATE
    GGML_USE_CPU
    WHISPER_NO_COREML
    WHISPER_NO_METAL
    WHISPER_NO_VULKAN
)

# Include directories
target_include_directories(native_whisper PRIVATE
    ${SRC_ROOT}
    ${GGML_DIR}
)

# Link to Android log and android libraries
find_library(log-lib log)
find_library(android-lib android)

target_link_libraries(native_whisper PRIVATE
    ${log-lib}
    ${android-lib}
    atomic
    m
)

# For NDK 23+ the atomic lib is typically available, keep it if needed
