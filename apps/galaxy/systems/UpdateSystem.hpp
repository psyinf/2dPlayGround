#pragma once
#include <entt/entt.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Vec.hpp>
#include <pgGame/entities/Drawable.hpp>
#include <entities/StarSystem.hpp>
#include <iostream>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class UpdateSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup() override{};

    void handle(const pg::game::FrameStamp& frameStamp) override
    {
        return; // no rotation for now
        auto& registry = game.getRegistry();
        auto  view = registry.view<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState>();
        for (auto& entity : view)
        {
            auto&& [drawable, transform, starSystemState] =
                view.get<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState>(entity);
            // move the stars on a circle at the distance from center
            auto distance = normalize(transform.pos);
            // auto angle = angleBetween(pg::fVec2{0, 1}, transform.pos);
            auto angle = atan2(transform.pos[1], transform.pos[0]);
            angle += 0.0001 * sqrt(1.0 / distance);
            transform.pos = {distance * cos(angle), distance * sin(angle)};
        }
        auto& fpsCounter = game.getApp().getFPSCounter();
        if (fpsCounter.getCurrentFrameCount() % 100 == 0) { std::cout << fpsCounter.getAverageFPSAndReset() << "\n"; }
    };
};
} // namespace galaxy