#include <pgEngine/core/ErrorTrace.hpp>
#include <pgGame/core/Game.hpp>
#include <fmt/format.h>

#include <components/Entities.h>
#include <systems/Asteroids.h>
#include <systems/Background.h>
#include <systems/Collisions.hpp>
#include <systems/DynamicsSystem.hpp>
#include <systems/Lasers.h>
#include <systems/Player.h>
#include <systems/RenderSystem.hpp>
#include <systems/SoundSystem.hpp>

#include <pgGame/systems/SystemsRegistry.hpp>
#include <pgGame/events/GameEvents.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    pg::game::Game game;

    pg::game::SystemsFactory::registerSystem<asteroids::Lasers>("lasers");
    pg::game::SystemsFactory::registerSystem<asteroids::Player>("player");
    pg::game::SystemsFactory::registerSystem<asteroids::Asteroids>("asteroids");
    pg::game::SystemsFactory::registerSystem<asteroids::Background>("background");
    pg::game::SystemsFactory::registerSystem<asteroids::Collisions>("collisions");
    pg::game::SystemsFactory::registerSystem<asteroids::RenderSystem>("renderSystem");
    pg::game::SystemsFactory::registerSystem<asteroids::DynamicsSystem>("dynamicsSystem");
    pg::game::SystemsFactory::registerSystem<asteroids::SoundSystem>("soundSystem");
    game.createScene({.scene_id = "start",
                      .systems{"lasers",
                               "player",
                               "asteroids",
                               "background",
                               "collisions",
                               "renderSystem",
                               "dynamicsSystem",
                               "soundSystem"}});
    game.switchScene("start");
    // send start event
    game.getGlobalDispatcher().enqueue<pg::game::events::PlayPauseEvent>(
        {.state = pg::game::events::PlayPauseEvent::State::Play});

    // TODO: from external config
    game.getCurrentScene().addSingleton<asteroids::RenderConfig>(
        asteroids::RenderConfig{.renderBroadPhaseCollisionShapes = true});

    game.loop();
    return 0;
}

catch (std::exception& e)
{
    fmt::print("Unhandled exception: {}\n", e.what());
    errorTrace::printErrorTrace();
    return -1;
}

catch (...)
{
    fmt::print("Unhandled exception\n");
    errorTrace::printErrorTrace();
    return -1;
}