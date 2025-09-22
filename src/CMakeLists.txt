# src/CMakeLists.txt
file(GLOB_RECURSE CORE_SOURCES   "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp")
file(GLOB_RECURSE LOGIC_SOURCES  "${CMAKE_CURRENT_SOURCE_DIR}/logic/*.cpp")
file(GLOB_RECURSE AI_SOURCES     "${CMAKE_CURRENT_SOURCE_DIR}/ai/*.cpp")

add_library(thai_core  ${CORE_SOURCES})
target_include_directories(thai_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(thai_core PUBLIC DATA_DIR="${CMAKE_SOURCE_DIR}/data")

add_library(thai_logic ${LOGIC_SOURCES}
        logic/probability_table.cpp
        logic/hand_table.cpp)
target_link_libraries(thai_logic PUBLIC thai_core)
target_compile_definitions(thai_logic PUBLIC DATA_DIR="${CMAKE_SOURCE_DIR}/data")

add_library(thai_ai ${AI_SOURCES}
        ai/hand_cluster.cpp
        ai/counterfactual_regret.cpp)
target_link_libraries(thai_ai PUBLIC thai_core thai_logic)

add_library(thai_poker INTERFACE)
target_link_libraries(thai_poker INTERFACE thai_core thai_logic)
target_link_libraries(thai_poker INTERFACE thai_ai)
target_include_directories(thai_poker INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
