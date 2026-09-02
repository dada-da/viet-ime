cmake_minimum_required(VERSION 3.20)

project(viet_ime
  VERSION 0.1.0
  DESCRIPTION "Cross-platform Vietnamese input method engine"
  LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(ime
  core/main.cpp
  core/tone_table.cpp
  core/telex.c
  core/key_processor.cpp
  core/config.c)

target_include_directories(ime PRIVATE core)

if(MSVC)
  target_compile_options(ime PRIVATE /W4 /utf-8)
else()
  target_compile_options(ime PRIVATE -Wall -Wextra)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC AND NOT WIN32)
    target_compile_options(ime PRIVATE -fsanitize=address,undefined)
    target_link_options(ime PRIVATE -fsanitize=address,undefined)
endif()