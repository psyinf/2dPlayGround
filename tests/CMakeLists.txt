project(unittests LANGUAGES CXX)
find_package(Catch2 REQUIRED)

# Tests
add_library(testMain OBJECT testMain.cpp)
target_link_libraries(testMain PUBLIC Catch2::Catch2)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
if(BUILD_TESTING)

    file( GLOB_RECURSE TEST_SOURCES *Tests.cpp )
    foreach( testsourcefile ${TEST_SOURCES} )
        # Cut off the file extension and directory path
        get_filename_component( target ${testsourcefile} NAME_WE )
        add_executable( ${target} ${testsourcefile} )
        # Make sure YourLib is linked to each app
        set_target_properties(${target} PROPERTIES 
            CXX_EXTENSIONS OFF
            BUILD_RPATH "${CMAKE_BINARY_DIR}/_deps/openal-build"
            # The main project globally skips RPATH; keep it enabled for tests so bundled OpenAL is found
            SKIP_RPATH FALSE
            SKIP_BUILD_RPATH FALSE
            INSTALL_RPATH_USE_LINK_PATH TRUE
        )
            if(UNIX AND NOT APPLE)
                # Ensure the built OpenAL shared library is discoverable at runtime for test binaries
                target_link_options(${target} PRIVATE -Wl,-rpath,${CMAKE_BINARY_DIR}/_deps/openal-build)
            endif()
            target_compile_features(${target} PUBLIC cxx_std_20)
            target_compile_options(${target} PRIVATE ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
            target_link_libraries(${target} PRIVATE testMain pgEngine::pgEngine pgGame::pgGame nlohmann_json::nlohmann_json INTERFACE pgOrbit::pgOrbit)

            #add_test(NAME ${target} COMMAND ${target})
            catch_discover_tests(
              ${target}
              TEST_PREFIX ${target}.
              REPORTER XML
              OUTPUT_DIR .
              OUTPUT_PREFIX ${target}
              OUTPUT_SUFFIX .xml
            )
    endforeach( testsourcefile ${APP_SOURCES} )

    
endif()