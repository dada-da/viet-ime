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

add_library(ime_core STATIC
  core/tone_table.cpp
  core/telex.c
  core/key_processor.cpp
  core/config.c
  core/utf8.cpp
  core/normalize.cpp)

target_include_directories(ime_core PUBLIC core)

add_executable(ime core/main.cpp)
target_link_libraries(ime PRIVATE ime_core)

enable_testing()

add_executable(ime_tests
  tests/check.cpp
  tests/test_main.cpp
  tests/test_utf8.cpp
  # tests/test_normalize.cpp
  )

target_link_libraries(ime_tests PRIVATE ime_core)
add_test(NAME core COMMAND ime_tests)

foreach(t ime_core ime ime_tests)
  if (MSVC)
    target_compile_options(${t} PRIVATE /W4 /utf-8)
  else()
    target_compile_options(${t} PRIVATE -Wall -Wextra)
  endif()
endforeach()
