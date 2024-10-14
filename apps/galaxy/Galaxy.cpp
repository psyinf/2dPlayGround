#include "Galaxy.hpp"
#include <scenes/GalaxyScene.hpp>
#include <scenes/SplashScreen.hpp>

#include <scenes/PreLoadResources.hpp>
#include <systems/RenderSystem.hpp>
#include <systems/SoundSystem.hpp>
#include <systems/GuiRenderSystem.hpp>
#include <scenes/SystemScene.hpp>

galaxy::GalacticCore::GalacticCore()
  : game(std::make_unique<pg::game::Game>())
{
}

void galaxy::GalacticCore::setup()
{
    // systems
    pg::game::SystemsFactory::registerSystem<galaxy::SoundSystem>("soundSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::RenderSystem>("renderSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::GuiRenderSystem>("guiSystem");

    // scenes
    game->createScene<galaxy::SplashScreen>("splashScreen", {.systems = {"soundSystem", "renderSystem", "guiSystem"}});
    game->createScene<galaxy::PreLoadResources>("loadGalaxy",
                                                {.systems = {"soundSystem", "renderSystem", "guiSystem"}});
    game->createScene<galaxy::GalaxyScene>("galaxy", {.systems = {"soundSystem", "renderSystem", "guiSystem"}});
    game->createScene<galaxy::SystemScene>("system", {.systems = {"soundSystem", "renderSystem", "guiSystem"}});
    auto& scene = game->switchScene("splashScreen");
    scene.start();
}

void galaxy::GalacticCore::run()
{
    game->loop();
}
