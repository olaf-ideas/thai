cmake_minimum_required(VERSION 3.20)
project(thai_poker LANGUAGES CXX)

# ---- Options ---------------------------------------------------
option(THAI_BUILD_TESTS "Build unit tests" ON)
option(THAI_BUILD_TOOLS "Build extra tools (table builders, etc.)" ON)
option(THAI_ENABLE_ASAN "Enable Address/UB sanitizers in Debug" ON)
option(THAI_ENABLE_CCACHE "Use ccache if available" ON)

# ---- Compiler standard & compilation database ------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- ccache (optional) -----------------------------------------
if (THAI_ENABLE_CCACHE)
  find_program(CCACHE_PROGRAM ccache)
  if (CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
  endif()
endif()

# ---- Global warnings & sanitizers/LTO --------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(warnings) # sets target warnings via function add_strict_warnings()

# Prefer LTO in Release for performance (GCC/Clang)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  include(CheckIPOSupported)
  check_ipo_supported(RESULT lto_supported)
  if (lto_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
  endif()
endif()

# Sanitizers (Debug only)
if (THAI_ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
  add_link_options(-fsanitize=address,undefined)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
endif()

# ---- Subdirectories --------------------------------------------
add_subdirectory(src)

# ---- Apps / Tools ----------------------------------------------
add_subdirectory(apps)

# ---- Tests -----------------------------------------------------
if (THAI_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()