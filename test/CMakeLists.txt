include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(tests_all ${TEST_SOURCES})
target_link_libraries(tests_all PRIVATE thai_poker GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests_all)  # ctest will auto-discover
