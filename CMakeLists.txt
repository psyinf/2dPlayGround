cmake_minimum_required(VERSION 3.22)
project(2dPlayground)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


#options
option(ENABLE_COVERAGE "Enable coverage" OFF)


#add cmake script to download conan-cmake integrations
#include(cmake/conan_config.cmake)
include(cmake/PreventInSourceBuilds.cmake)
if (ENABLE_COVERAGE)
    include(cmake/coverage.cmake)
endif()
include(cmake/stacktrace.cmake)
include(cmake/setup_cpm.cmake)
include(cmake/deps_cpm.cmake)



enable_testing()
add_subdirectory(libs)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(tools)
install(DIRECTORY
        data/
    DESTINATION
        data
    COMPONENT
        ${RUNTIME_COMPONENT}
)