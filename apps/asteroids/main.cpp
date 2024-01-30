#include <SDLErrorTrace.h>
#include <core/Game.hpp>
#include <fmt/format.h>

#include <entities/Entities.h>
#include <systems/Asteroids.h>
#include <systems/Background.h>
#include <systems/Collisions.hpp>
#include <systems/DynamicsSystem.hpp>
#include <systems/Lasers.h>
#include <systems/Player.h>
#include <systems/RenderSystem.hpp>


int main(int argc, char** argv)
try
{
    pg::game::Game game;
    auto&          systems = game.getSystems();
    systems.emplace_back(std::make_unique<game::Lasers>(game));
    systems.emplace_back(std::make_unique<game::Player>(game));
    systems.emplace_back(std::make_unique<game::Asteroids>(game));
    systems.emplace_back(std::make_unique<game::Background>(game));
    systems.emplace_back(std::make_unique<game::Collisions>(game));
    systems.emplace_back(std::make_unique<game::RenderSystem>(game));
    systems.emplace_back(std::make_unique<game::DynamicsSystem>(game));
    game.setup();

    auto playerId = game.getSingleton<const entt::entity>("Player");
    auto event = game::events::LaserFired{.offset{}, .shooter = playerId};
    auto trigger = [event, &game](auto) { game.getDispatcher().trigger(event); };
    game.getKeyStateMap().registerDirectCallback(SDLK_SPACE, {pg::KeyStateMap::CallbackTrigger::RELEASED, trigger});

    // TODO: from external config
    game.getRegistry().ctx().emplace<game::RenderConfig>(game::RenderConfig{.renderBroadPhaseCollisionShapes = true});

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
    fmt::print("Unhandled exception");
    errorTrace::printErrorTrace();
    return -1;
}