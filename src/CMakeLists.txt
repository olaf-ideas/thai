# Collect sources per module. If you prefer explicit lists, replace the GLOBs.
file(GLOB_RECURSE CORE_SOURCES  CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp")
file(GLOB_RECURSE LOGIC_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/logic/*.cpp")
file(GLOB_RECURSE UTILS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp")
file(GLOB_RECURSE AI_SOURCES    CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ai/*.cpp")

# Each module is its own library (scales well for big projects)
add_library(thai_core  ${CORE_SOURCES})
add_library(thai_logic ${LOGIC_SOURCES})
add_library(thai_utils ${UTILS_SOURCES})

# AI is optional: only create if sources exist
if (AI_SOURCES)
  add_library(thai_ai ${AI_SOURCES})
endif()

# Where headers live: for now include the whole repo root so you can include "../core/..."
# Later you can move public headers into include/ and switch to that.
target_include_directories(thai_core  PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(thai_logic PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(thai_utils PUBLIC "${CMAKE_SOURCE_DIR}")
if (TARGET thai_ai)
  target_include_directories(thai_ai PUBLIC "${CMAKE_SOURCE_DIR}")
endif()

# Warnings on each target
add_strict_warnings(thai_core)
add_strict_warnings(thai_logic)
add_strict_warnings(thai_utils)
if (TARGET thai_ai)
  add_strict_warnings(thai_ai)
endif()

# Linkage between modules (tweak as needed)
target_link_libraries(thai_logic PUBLIC thai_core thai_utils)
if (TARGET thai_ai)
  target_link_libraries(thai_ai PUBLIC thai_core thai_utils)
endif()

# Umbrella library apps/tests can link against.
add_library(thai_poker INTERFACE)
target_link_libraries(thai_poker INTERFACE thai_core thai_logic thai_utils)
if (TARGET thai_ai)
  target_link_libraries(thai_poker INTERFACE thai_ai)
endif()

# POSIX threads if you use them
if (UNIX)
  find_package(Threads REQUIRED)
  target_link_libraries(thai_core  PUBLIC Threads::Threads)
  target_link_libraries(thai_logic PUBLIC Threads::Threads)
  target_link_libraries(thai_utils PUBLIC Threads::Threads)
  if (TARGET thai_ai)
    target_link_libraries(thai_ai PUBLIC Threads::Threads)
  endif()
endif()
