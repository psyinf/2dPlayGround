project(markov)

set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE HEADER_FILES 	CONFIGURE_DEPENDS "*.h*")
file(GLOB_RECURSE CPP_FILES 	CONFIGURE_DEPENDS "*.cpp")

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${CPP_FILES} )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    pgEngine::pgEngine
	imgui
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