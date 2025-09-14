cmake_minimum_required(VERSION 3.20)
project(thai_poker LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(THAI_BUILD_TESTS "Build GoogleTest-based tests" ON)
option(THAI_ENABLE_ASAN "Enable Address/Undefined sanitizers in Debug-like builds" OFF)

# ccache (optional)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Export compile_commands.json (good for clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Warnings (tweak to taste)
if(MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Sanitizers (Linux/macOS clang/gcc)
if(THAI_ENABLE_ASAN AND NOT MSVC)
  add_link_options(-fsanitize=address,undefined)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
endif()

# Subdirs (adjust to your structure)
add_subdirectory(src)
add_subdirectory(apps)

if(THAI_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
