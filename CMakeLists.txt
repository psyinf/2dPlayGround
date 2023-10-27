cmake_minimum_required(VERSION 3.22)
project(sagan)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#add cmake script to download conan-cmake integrations
include(cmake/conan_config.cmake)
include(cmake/PreventInSourceBuilds.cmake)
include(cmake/coverage.cmake)
include(cmake/stacktrace.cmake)
include(cmake/setup_cpm.cmake)

conan_cmake_configure(REQUIRES 
    catch2/3.4.0
    sdl/2.28.3
    GENERATORS CMakeDeps CMakeToolchain
)

conan_cmake_autodetect(settings)
conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})


CPMAddPackage(
NAME sdlpp    

GITHUB_REPOSITORY "mika314/sdlpp"
GIT_TAG HEAD
#OPTIONS USE_SDLGFX
)
enable_testing()
add_subdirectory(libs)
add_subdirectory(apps)
add_subdirectory(tests)

#install(DIRECTORY
#		data/
#	DESTINATION
#		{CMAKE_INSTALL_BINDIR}/data#
#	COMPONENT
#		${RUNTIME_COMPONENT}
#)