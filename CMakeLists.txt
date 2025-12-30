cmake_minimum_required(VERSION 3.22)
project(2dPlayground)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#some global compiler settings
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    #add_compile_options(/W4 /WX)
    add_compile_options(/W3)
    add_compile_options(/bigobj)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wno-c++98-compat -Wno-c++98-compat-pedantic)
else()
    add_compile_options(-Wall)
endif()
#options
option(ENABLE_COVERAGE "Enable coverage" OFF)
option(ENABLE_SANDBOXES "Enable sandbox projects" OFF)

#add cmake script to download conan-cmake integrations
#include(cmake/conan_config.cmake)
include(cmake/PreventInSourceBuilds.cmake)
if (ENABLE_COVERAGE)
    include(cmake/coverage.cmake)
endif()
include(cmake/stacktrace.cmake)
include(cmake/setup_cpm.cmake)
include(cmake/deps_cpm.cmake)
include(cmake/disable_3rd_party_warnings.cmake)


enable_testing()
add_subdirectory(libs)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(tools)

# Apply system includes to all external dependencies after they're all loaded
make_all_targets_system()
install(DIRECTORY
        data/
    DESTINATION
        data
    COMPONENT
        ${RUNTIME_COMPONENT}
)