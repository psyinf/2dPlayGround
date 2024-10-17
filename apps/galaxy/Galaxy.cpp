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
    pg::game::SystemsFactory::registerSystem<galaxy::TaggedRenderSystem<pg::tags::GalaxyRenderTag>>(
        "galaxyRenderSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::TaggedRenderSystem<pg::tags::SystemRenderTag>>(
        "systemRenderSystem");

    // scenes
    game->createScene<galaxy::SplashScreen>("splashScreen", {.systems = {"soundSystem", "guiSystem"}});
    game->createScene<galaxy::PreLoadResources>("loadGalaxy", {.systems = {"soundSystem", "guiSystem"}});
    game->createScene<galaxy::GalaxyScene>("galaxy", {.systems = {"soundSystem", "galaxyRenderSystem", "guiSystem"}});
    game->createScene<galaxy::SystemScene>("system", {.systems = {"soundSystem", "systemRenderSystem", "guiSystem"}});

    game->addSingleton_as<pg::game::SystemInterface::Config>("guiSystem.loadGalaxy.config",
                                                             pg::game::SystemInterface::Config{{"standalone", "true"}});
    game->addSingleton_as<pg::game::SystemInterface::Config>(
        "guiSystem.system.config", pg::game::SystemInterface::Config{{"standalone", "true"}});
    auto& scene = game->switchScene("galaxy");
    scene.start();
}

void galaxy::GalacticCore::run()
{
    game->loop();
}
