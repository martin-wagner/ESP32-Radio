
set( CMAKE_CXX_FLAGS "-Wall -o0 -g " )

add_executable(
  sm_webctrl
  ../src/sm_webctrl.cpp
  log.cpp
  test_sm_webctrl_history.cpp
  test_sm_webctrl_statistics.cpp
)
target_include_directories(
  sm_webctrl
  PRIVATE
  ${PROJECT_SOURCE_DIR}
)
target_link_libraries(
  sm_webctrl
  gtest_main
  gmock
)

include(GoogleTest)
gtest_discover_tests(sm_webctrl)

# building and running:
# cd . (project root)
# mkdir build/test
# cd build/test
# cmake ../..
# make && ctest
