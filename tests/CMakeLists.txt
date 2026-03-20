include(GoogleTest)

option(DSA_ENABLE_LEARNER_TESTS "Build learner algorithm tests" OFF)

add_executable(dsa_tests test_viz_recordings.cpp)
target_link_libraries(dsa_tests PRIVATE viz GTest::gtest_main)
gtest_discover_tests(dsa_tests)

if(DSA_ENABLE_LEARNER_TESTS)
  add_executable(dsa_learner_tests test_linear_search.cpp test_binary_search.cpp
                                   test_bubble_sort.cpp)
  target_link_libraries(dsa_learner_tests PRIVATE dsa GTest::gtest_main)
  gtest_discover_tests(dsa_learner_tests)
endif()
