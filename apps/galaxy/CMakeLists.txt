project(galaxy)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE HEADER_FILES 	CONFIGURE_DEPENDS "*.h*")
file(GLOB_RECURSE CPP_FILES 	CONFIGURE_DEPENDS "*.cpp")


add_executable(${PROJECT_NAME} ${HEADER_FILES} ${CPP_FILES})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    pgEngine::pgEngine
    pgGame::pgGame
    pgOrbit::pgOrbit
    spdlog::spdlog
    magic_enum::magic_enum
    BT::behaviortree_cpp
    soundEngine::soundEngine
    pgf::pgf
    IconFontCppHeaders
    INTERFACE
    pgFoundation::pgFoundation
    ${ADDITION_LIBS}
)
target_include_directories(${PROJECT_NAME}
	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

if (ENABLE_COVERAGE)
    enable_coverage(${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>)

set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME} )
