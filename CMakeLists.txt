cmake_minimum_required(VERSION 3.22)
project(2dPlayground)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#add cmake script to download conan-cmake integrations
include(cmake/conan_config.cmake)
include(cmake/PreventInSourceBuilds.cmake)
include(cmake/coverage.cmake)
include(cmake/stacktrace.cmake)
include(cmake/setup_cpm.cmake)
include(cmake/deps_cpm.cmake)

conan_cmake_configure(REQUIRES 
    #catch2/3.4.0
    sdl/2.28.3
    sdl_image/2.6.3
    sdl_ttf/2.20.2
    #fmt/10.1.1
    #entt/3.12.2
    libpng/1.6.42 #resolve conflicts
    GENERATORS CMakeDeps CMakeToolchain
)

conan_cmake_autodetect(settings)
conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})



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