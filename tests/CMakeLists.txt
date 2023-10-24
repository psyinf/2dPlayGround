cmake_minimum_required(VERSION 3.15...3.23)
project(tests LANGUAGES CXX)
find_package(Catch2 CONFIG REQUIRED)

add_executable(tests tests.cpp)

target_link_libraries(
  tests
  PRIVATE 
          Catch2::Catch2WithMain)

          
include(CTest)
include(Catch)
catch_discover_tests(
  tests
  TEST_PREFIX "unittests."
  REPORTER XML
  OUTPUT_DIR .
  OUTPUT_PREFIX "unittests."
  OUTPUT_SUFFIX .xml
)
