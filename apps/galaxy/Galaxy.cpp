#include "Galaxy.hpp"
#include <scenes/GalaxyScene.h>

#include <pgEngine/core/LoadSave.hpp>

galaxy::GalacticCore::GalacticCore()
  : game(std::make_unique<pg::game::Game>())
{
}

void galaxy::GalacticCore::setup()
{
    auto& scene = game->createAndSwitchScene<galaxy::GalaxyScene>("galaxy");

    scene.start();
}

void galaxy::GalacticCore::run()
{
    game->loop();
}
