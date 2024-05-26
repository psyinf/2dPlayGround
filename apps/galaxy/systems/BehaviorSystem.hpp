#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <components/Behavior.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class BehaviorSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup() {}

    void handle(const pg::game::FrameStamp& frameStamp)
    {
        auto view = game.getRegistry().view<galaxy::Behavior>();
        for (auto entity : view)
        {
            auto& behavior = view.get<galaxy::Behavior>(entity);
            behavior.tree.tickExactlyOnce();
        }
        // delay deletions after the update
        game.getDispatcher().update();
    };
};
} // namespace galaxy