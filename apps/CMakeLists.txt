# apps/CMakeLists.txt
add_executable(thai_cli apps_main.cpp)        # your driver
target_link_libraries(thai_cli PRIVATE thai_poker)

add_executable(precompute_P precompute_P.cpp) # e.g., writes the P table
target_link_libraries(precompute_P PRIVATE thai_poker)
