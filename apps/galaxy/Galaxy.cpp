#include "Galaxy.hpp"
#include <scenes/GalaxyScene.hpp>
#include <scenes/SplashScreen.hpp>

#include <scenes/PreLoadResources.hpp>
#include <systems/RenderSystem.hpp>
#include <systems/SoundSystem.hpp>
#include <systems/GuiRenderSystem.hpp>
#include <scenes/SystemScene.hpp>
#include <configs/SceneConfig.hpp>

galaxy::GalacticCore::GalacticCore()
  : game(std::make_unique<pg::game::Game>())
{
}

void galaxy::GalacticCore::setup()
{
    game->getConfig().addPerSceneConfig<galaxy::SceneSoundScapeConfig>(
        "splashScreen", "soundScape", {{"../data/music/dead-space-style-ambient-music-184793.mp3"}});

    game->getConfig().addPerSceneConfig<galaxy::SceneSoundScapeConfig>(
        "galaxy", "soundScape", {{"../data/music/a-meditation-through-time-amp-space-11947.mp3"}});
    // systems
    pg::game::SystemsFactory::registerSystem<galaxy::SoundSystem>("soundSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::RenderSystem>("renderSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::GuiRenderSystem>("guiSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::TaggedRenderSystem<pg::tags::GalaxyRenderTag>>(
        "galaxyRenderSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::TaggedRenderSystem<pg::tags::SystemRenderTag>>(
        "systemRenderSystem");

    pg::game::SystemsFactory::registerSystem<galaxy::UpdateStarsSystem>("updateStarsSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::PickingSystem>("pickingSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::DroneSystem>("droneSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::LifetimeSystem>("lifeTimeSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::BehaviorSystem>("behaviorSystem");
    pg::game::SystemsFactory::registerSystem<galaxy::StatsSystem>("statsSystem");

    // scenes
    game->createScene<galaxy::SplashScreen>("splashScreen", {.systems = {"soundSystem", "guiSystem"}});
    game->createScene<galaxy::PreLoadResources>("loadGalaxy", {.systems = {"soundSystem", "guiSystem"}});
    game->createScene<galaxy::GalaxyScene>("galaxy",
                                           {.systems = {"soundSystem",
                                                        "galaxyRenderSystem",
                                                        "guiSystem",
                                                        "updateStarsSystem",
                                                        "pickingSystem",
                                                        "droneSystem",
                                                        "lifeTimeSystem",
                                                        "behaviorSystem",
                                                        "statsSystem"}});

    game->createScene<galaxy::SystemScene>("system", {.systems = {"soundSystem", "systemRenderSystem", "guiSystem"}});

    game->addSingleton_as<pg::game::SystemInterface::Config>("guiSystem.loadGalaxy.config",
                                                             pg::game::SystemInterface::Config{{"standalone", "true"}});
    game->addSingleton_as<pg::game::SystemInterface::Config>(
        "guiSystem.system.config", pg::game::SystemInterface::Config{{"standalone", "false"}});

    auto& scene = game->switchScene("splashScreen");
    scene.start();
}

void galaxy::GalacticCore::run()
{
    game->loop();
}
