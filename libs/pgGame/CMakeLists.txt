project(pgGame)

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
 
find_package(EnTT REQUIRED)

target_sources(${PROJECT_NAME}
	PRIVATE
		core/Game.cpp
		core/KeyStateMap.cpp
		core/Scene.cpp
		systems/SystemInterface.cpp
		core/VFSDataProvider.cpp
	PUBLIC
		core/Game.hpp
		core/RegistryHelper.hpp
		core/KeyStateMap.hpp
		core/Scene.hpp
		
		systems/SystemInterface.hpp
		systems/SystemsRegistry.hpp
		systems/LambdaSystem.hpp
		components/WindowDetails.hpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
		EnTT::EnTT
	    spdlog::spdlog
		pgf::pgf
	PRIVATE
	    pgEngine::pgEngine
		#vfspp::vfspp

)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>)

