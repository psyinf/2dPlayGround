#pragma once
#include <pgGame/core/Game.hpp>
#include <pgGame/core/RegistryHelper.hpp>
#include <pgGame/entities/Drawable.hpp>
#include <pgEngine/primitives/Sprite.hpp>
#include <systems/RenderSystem.hpp>
#include <systems/UpdateSystem.hpp>

#include "entities/StarSystem.hpp"
#include <pgEngine/math/Random.hpp>
#include <pgGame/entities/WindowDetails.hpp>

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
        auto& scene = game->createScene("start", true);
        auto& systems = scene.getSystems();
        systems.emplace_back(std::make_unique<galaxy::RenderSystem>(*game));
        systems.emplace_back(std::make_unique<galaxy::UpdateSystem>(*game));

        setupStarSystems();
    }

    void run() { game->loop(); }

private:
    void setupStarSystems()
    {
        auto dot_sprite = game->getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");

        for (auto i : std::ranges::iota_view{1, 5000})
        {
            pg::game::makeEntity<pg::Transform2D, pg::game::Drawable, galaxy::StarSystemState>(
                game->getRegistry(),
                {.pos{pg::getPointInCircle(375)}, .scale{0.025, 0.025}},
                pg::game::Drawable{dot_sprite},
                galaxy::StarSystemState{});
        }
    }

private:
    std::unique_ptr<pg::game::Game> game;
};

} // namespace galaxy