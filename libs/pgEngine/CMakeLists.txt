project(pgEngine)
find_package(SDL2 REQUIRED)
#find_package(SDL2_image REQUIRED)
#find_package(SDL2_ttf REQUIRED)
find_package(soundEngine REQUIRED)

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		"core/App.cpp"
		"primitives/Renderable.cpp"
		"primitives/Sprite.cpp"

		factories/Factories.cpp
		font/Font.cpp
		"core/FPSCounter.cpp"
		core/Gui.cpp
	PUBLIC
		factories/Factories.hpp
		
		font/Font.hpp
		
		math/Bounds.hpp
		math/BoundsOperations.hpp
		math/Box.hpp
		math/ComputationalGeometry.hpp 
		math/Random.hpp
		math/Vec.hpp
		math/VecOps.hpp
		primitives/Renderable.hpp

		primitives/BackgoundSprite.hpp
		primitives/Sprite.hpp
		sprite/SpritePixelData.hpp

		"core/App.hpp"
		"core/AppConfig.hpp"
		core/FPSCounter.hpp
		

		
		"core/ErrorTrace.hpp"


)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    sdlpp::sdlpp
    SDL2::SDL2
    SDL2::SDL2main
	SDL2_image::SDL2_image-static
	nlohmann_json::nlohmann_json
    imgui
	PRIVATE
	SDL2_ttf::SDL2_ttf-static
	soundEngine::soundEngine
)

target_include_directories(
  ${PROJECT_NAME}
		
  PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)