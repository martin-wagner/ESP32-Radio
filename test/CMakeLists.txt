
add_executable(
  dummy
  test.cpp
)
target_link_libraries(
  dummy
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(dummy)

# building and running:
# cd . (project root)
# mkdir build/test
# cd build/test
# cmake ../..
# make && ctest
