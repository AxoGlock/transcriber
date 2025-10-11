# Install script for directory: C:/transcriber/app/src/main/cpp/whispercpp/ggml

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/native_whisper")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Users/axoglock/AppData/Local/Android/SDK/ndk/26.1.10909125/toolchains/llvm/prebuilt/windows-x86_64/bin/llvm-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/transcriber/app/.cxx/Debug/4r4le1q6/armeabi-v7a/whispercpp/ggml/src/cmake_install.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/transcriber/app/build/intermediates/cxx/Debug/4r4le1q6/obj/armeabi-v7a/libggml.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/Users/axoglock/AppData/Local/Android/SDK/ndk/26.1.10909125/toolchains/llvm/prebuilt/windows-x86_64/bin/llvm-strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-cpu.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-alloc.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-backend.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-blas.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-cann.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-cpp.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-cuda.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-opt.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-metal.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-rpc.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-sycl.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-vulkan.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/ggml-webgpu.h"
    "C:/transcriber/app/src/main/cpp/whispercpp/ggml/include/gguf.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml-base.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml-base.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml-base.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/transcriber/app/build/intermediates/cxx/Debug/4r4le1q6/obj/armeabi-v7a/libggml-base.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml-base.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml-base.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/Users/axoglock/AppData/Local/Android/SDK/ndk/26.1.10909125/toolchains/llvm/prebuilt/windows-x86_64/bin/llvm-strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libggml-base.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ggml" TYPE FILE FILES
    "C:/transcriber/app/.cxx/Debug/4r4le1q6/armeabi-v7a/whispercpp/ggml/ggml-config.cmake"
    "C:/transcriber/app/.cxx/Debug/4r4le1q6/armeabi-v7a/whispercpp/ggml/ggml-version.cmake"
    )
endif()

