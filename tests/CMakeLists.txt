cmake_minimum_required(VERSION 3.15...3.23)
project(tests LANGUAGES CXX)
find_package(Catch2 CONFIG REQUIRED)


add_library(test_sources OBJECT 
    ResourceCache/ResourceCacheTests.cpp
    Bounds/BoundsTest.cpp
)



target_link_libraries(
    test_sources   
    PUBLIC
        pgEngine::pgEngine
        Catch2::Catch2
)

add_executable(tests tests.cpp)

target_link_libraries(
  tests
  PRIVATE 
    test_sources
    Catch2::Catch2


)

          
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
