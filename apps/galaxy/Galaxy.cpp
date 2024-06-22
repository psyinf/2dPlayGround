#include "Galaxy.hpp"
#include <scenes/GalaxyScene.hpp>
#include <scenes/SplashScreen.hpp>

galaxy::GalacticCore::GalacticCore()
  : game(std::make_unique<pg::game::Game>())
{
}

void galaxy::GalacticCore::setup()
{
    game->createScene<galaxy::SplashScreen>("splashScreen");
    game->createScene<galaxy::GalaxyScene>("galaxy");

    auto& scene = game->switchScene("splashScreen");
    scene.start();
}

void galaxy::GalacticCore::run()
{
    game->loop();
}
