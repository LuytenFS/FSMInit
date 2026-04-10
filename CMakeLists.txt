cmake_minimum_required(VERSION 3.16)
project(FSMINIT LANGUAGES C)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# More reliable source listing (avoids globbing issues)
set(SRCS src/main.c src/file_subsystem.c)

add_executable(FSMINIT ${SRCS})
target_include_directories(FSMINIT PRIVATE include)

# Cross-platform compile options
if(MSVC)
  target_compile_options(FSMINIT PRIVATE
    /W4
    $<$<CONFIG:Debug>:/Zi /Od>
    $<$<CONFIG:Release>:/O2>
  )
  target_compile_definitions(FSMINIT PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
  )
else()
  target_compile_options(FSMINIT PRIVATE
    -Wall -Wextra
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O2>
  )
endif()

# Enhanced clang-tidy detection and target
find_program(CLANG_TIDY_EXE
  NAMES clang-tidy clang-tidy.exe
  PATHS
    /usr/bin
    /usr/local/bin
    "C:/Program Files/LLVM/bin"
    "C:/chocolatey/lib/llvm/tools/clang/bin"
  DOC "Path to clang-tidy executable"
)
if(CLANG_TIDY_EXE)
  add_custom_target(tidy
    COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${SRCS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy..."
    VERBATIM
  )
  message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
else()
  message(WARNING "clang-tidy not found, 'tidy' target unavailable")
endif()

install(TARGETS FSMINIT RUNTIME DESTINATION bin)
