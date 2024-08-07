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

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    pg::game::Game game;
    game.createScene("start", {});
    auto& scene = game.switchScene("start");
    auto& systems = scene.getSystems();
    systems.emplace_back(std::make_unique<asteroids::Lasers>(game));
    systems.emplace_back(std::make_unique<asteroids::Player>(game));
    systems.emplace_back(std::make_unique<asteroids::Asteroids>(game));
    systems.emplace_back(std::make_unique<asteroids::Background>(game));
    systems.emplace_back(std::make_unique<asteroids::Collisions>(game));
    systems.emplace_back(std::make_unique<asteroids::RenderSystem>(game));
    systems.emplace_back(std::make_unique<asteroids::DynamicsSystem>(game));
    systems.emplace_back(std::make_unique<asteroids::SoundSystem>(game));
    game.switchScene("start");

    // TODO: from external config
    game.getCurrentScene().addSingleton<asteroids::RenderConfig>(
        asteroids::RenderConfig{.renderBroadPhaseCollisionShapes = true});
    scene.start();
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