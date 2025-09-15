# src/CMakeLists.txt
file(GLOB_RECURSE CORE_SOURCES   "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp")
file(GLOB_RECURSE LOGIC_SOURCES  "${CMAKE_CURRENT_SOURCE_DIR}/logic/*.cpp")
file(GLOB_RECURSE AI_SOURCES     "${CMAKE_CURRENT_SOURCE_DIR}/ai/*.cpp")

add_library(thai_core  ${CORE_SOURCES})
target_include_directories(thai_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(thai_logic ${LOGIC_SOURCES}
        logic/probability_table.cpp)
target_link_libraries(thai_logic PUBLIC thai_core)

if(AI_SOURCES)
  add_library(thai_ai ${AI_SOURCES})
  target_link_libraries(thai_ai PUBLIC thai_core thai_logic)
endif()

# Umbrella interface for “everything”
add_library(thai_poker INTERFACE)
target_link_libraries(thai_poker INTERFACE thai_core thai_logic)
if(TARGET thai_ai)
  target_link_libraries(thai_poker INTERFACE thai_ai)
endif()
target_include_directories(thai_poker INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
