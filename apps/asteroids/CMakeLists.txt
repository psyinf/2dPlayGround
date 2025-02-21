cmake_minimum_required(VERSION 3.22)
project(asteroids)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE HEADER_FILES 	CONFIGURE_DEPENDS "*.h*")
file(GLOB_RECURSE CPP_FILES 	CONFIGURE_DEPENDS "*.cpp")


add_executable(${PROJECT_NAME} ${HEADER_FILES} ${CPP_FILES} )

find_package(EnTT REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    pgEngine::pgEngine
    pgGame::pgGame
    soundEngine::soundEngine
    pybind11::embed
    pybind11::pybind11
    pybind11::headers

    pgf::pgf
    EnTT::EnTT
    INTERFACE
    pgFoundation::pgFoundation

)
target_include_directories(${PROJECT_NAME}
	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

if (ENABLE_COVERAGE)
    enable_coverage(${PROJECT_NAME})
endif()
install(DIRECTORY
        data/
    DESTINATION
        data
    COMPONENT
        ${RUNTIME_COMPONENT}
)
install(TARGETS ${PROJECT_NAME})