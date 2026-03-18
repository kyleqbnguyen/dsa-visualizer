include(GoogleTest)

add_executable(dsa_tests test_linear_search.cpp test_binary_search.cpp
                         test_bubble_sort.cpp)

target_link_libraries(dsa_tests PRIVATE dsa GTest::gtest_main)

gtest_discover_tests(dsa_tests)
