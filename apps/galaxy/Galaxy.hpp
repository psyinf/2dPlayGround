#pragma once
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/entities/Drawable.hpp>
#include <systems/RenderSystem.hpp>
#include "entities/StarSystem.hpp"

namespace galaxy {
class GalacticCore
{
public:
    GalacticCore()
      : game(std::make_unique<pg::game::Game>())
    {
    }

    void setup()
    {
        auto& systems = game->getSystems();
        systems.emplace_back(std::make_unique<galaxy::RenderSystem>(*game));

        game->setup();
    }

    void run() { game->loop(); }

private:
    void setupStarSystems()
    {
        pg::game::makeEntity<pg::game::Drawable, galaxy::StarSystemState>(game->getRegistry(), {}, {});
    }

private:
    std::unique_ptr<pg::game::Game> game;
};

} // namespace galaxy