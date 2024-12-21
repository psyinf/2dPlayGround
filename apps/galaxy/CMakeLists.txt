project(galaxy)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE HEADER_FILES 	CONFIGURE_DEPENDS "*.h*")
file(GLOB_RECURSE CPP_FILES 	CONFIGURE_DEPENDS "*.cpp")


add_executable(${PROJECT_NAME} ${HEADER_FILES} ${CPP_FILES})

find_package(fmt REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    pgEngine::pgEngine
    pgGame::pgGame
    pgOrbit::pgOrbit
    fmt::fmt
    spdlog::spdlog
    magic_enum::magic_enum
    BT::behaviortree_cpp
    soundEngine::soundEngine
    pgf::pgf
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