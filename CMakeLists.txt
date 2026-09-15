# SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.20)

project(viet_ime
  VERSION 0.1.0
  DESCRIPTION "Cross-platform Vietnamese input method engine"
  LANGUAGES C CXX)

option(VIETIME_SANITIZE "Bật ASan/UBSan cho build Debug" ON)
option(VIETIME_COVERAGE "Bật đo coverage (gcov)" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

if(VIETIME_SANITIZE AND VIETIME_COVERAGE)
  message(FATAL_ERROR
    "VIETIME_SANITIZE và VIETIME_COVERAGE loại trừ nhau. Chọn một.")
endif()

# Số ABI: đọc thẳng từ core/ime_api.h
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/core/ime_api.h" VIETIME_ABI_LINE
  REGEX "^#define[ \t]+VIETIME_ABI_VERSION[ \t]+[0-9]+")
string(REGEX REPLACE "^#define[ \t]+VIETIME_ABI_VERSION[ \t]+([0-9]+).*$" "\\1"
  VIETIME_ABI_VERSION "${VIETIME_ABI_LINE}")

if(NOT VIETIME_ABI_VERSION MATCHES "^[0-9]+$")
  message(FATAL_ERROR
    "Không đọc được VIETIME_ABI_VERSION từ core/ime_api.h")
endif()
message(STATUS "vietime ABI version: ${VIETIME_ABI_VERSION}")

# ime_core — thư viện tĩnh nội bộ. Không ai ngoài repo này được link vào nó.
# KHÔNG chứa ime_api.cpp: file đó thuộc target vietime.
add_library(ime_core STATIC
  core/tone_table.cpp
  core/key_processor.cpp
  core/config.c
  core/utf8.cpp
  core/normalize.cpp
  core/syllable.cpp
  core/transform_engine.cpp
  core/case_map.cpp
  core/api_util.cpp
)
target_include_directories(ime_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/core")

set_target_properties(ime_core PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)

# vietime — thư viện động công khai
add_library(vietime SHARED
  core/ime_api.cpp
)

target_include_directories(vietime PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/core")

target_link_libraries(vietime PRIVATE ime_core)

target_compile_definitions(vietime PRIVATE VIETIME_EXPORTS)

set_target_properties(vietime PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  VERSION ${PROJECT_VERSION}
  SOVERSION ${VIETIME_ABI_VERSION}
)

# Chương trình thử nghiệm nội bộ
add_executable(ime core/main.cpp)
target_link_libraries(ime PRIVATE ime_core)

enable_testing()

# ime_tests — test cho logic bên trong core. Link ime_core TĨNH, không
# đụng tới libvietime.
add_executable(ime_tests
  tests/check.cpp
  tests/test_main.cpp
  tests/test_utf8.cpp
  tests/test_normalize.cpp
  tests/test_tone_table.cpp
  tests/test_key_processor.cpp
  tests/test_syllable.cpp
  tests/test_telex.cpp
  tests/test_vni.cpp
  tests/test_uppercase.cpp
  tests/test_case_map.cpp
  tests/test_undo.cpp
  tests/test_api_util.cpp
  tests/test_unicode.cpp
)
target_link_libraries(ime_tests PRIVATE ime_core)
target_include_directories(ime_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")
add_test(NAME core COMMAND ime_tests)

add_executable(api_tests
  tests/check.cpp
  tests/api_test_main.cpp
  tests/test_edge_case.cpp
  tests/test_api_contract.cpp
)
target_link_libraries(api_tests PRIVATE vietime)
target_include_directories(api_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")
add_test(NAME api COMMAND api_tests)

add_executable(type_sentence tests/type_sentence.c)
target_link_libraries(type_sentence PRIVATE vietime)
add_test(NAME c_sentence COMMAND type_sentence)

add_executable(fuzz_keys tests/fuzz_keys.cpp)
target_link_libraries(fuzz_keys PRIVATE vietime)
add_test(NAME fuzz COMMAND fuzz_keys 1 20000)

if(VIETIME_SANITIZE AND NOT MSVC AND NOT MINGW)
  set_tests_properties(core api c_sentence fuzz PROPERTIES
    ENVIRONMENT "ASAN_OPTIONS=detect_leaks=1:abort_on_error=1;UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1")
endif()

set_tests_properties(fuzz PROPERTIES TIMEOUT 300)

foreach(t ime_core vietime ime ime_tests api_tests type_sentence fuzz_keys)
  if(MSVC)
    target_compile_options(${t} PRIVATE /W4 /utf-8 $<$<CONFIG:Debug>:/WX>)
  else()
    target_compile_options(${t} PRIVATE
      -Wall -Wextra -Wshadow
      $<$<CONFIG:Debug>:-Werror>)

    if(NOT MINGW AND NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
      target_compile_options(${t} PRIVATE
        "$<$<AND:$<CONFIG:Debug>,$<BOOL:${VIETIME_SANITIZE}>>:-fsanitize=address,undefined;-fno-sanitize-recover=all;-fno-omit-frame-pointer;-g>")
      target_link_options(${t} PRIVATE
        "$<$<AND:$<CONFIG:Debug>,$<BOOL:${VIETIME_SANITIZE}>>:-fsanitize=address,undefined>")
    endif()

    if(VIETIME_COVERAGE)
      target_compile_options(${t} PRIVATE --coverage -O0 -g)
      target_link_options(${t} PRIVATE --coverage)
    endif()
  endif()
endforeach()
