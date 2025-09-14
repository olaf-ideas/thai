include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
# prevents MSVC CRT mismatch; harmless elsewhere
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# All test sources
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(tests_all ${TEST_SOURCES})
target_link_libraries(tests_all PRIVATE thai_poker GTest::gtest_main)
add_strict_warnings(tests_all)

include(GoogleTest)
gtest_discover_tests(tests_all)