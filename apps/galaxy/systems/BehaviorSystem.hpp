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

    void setup(std::string_view /*scene_id*/) {}

    void handle(const pg::game::FrameStamp&)
    {
        auto view = _game.getGlobalRegistry().view<galaxy::Behavior>();
        for (auto entity : view)
        {
            auto& behavior = view.get<galaxy::Behavior>(entity);
            auto  res = behavior.tree.tickExactlyOnce();
            if (res == BT::NodeStatus::SUCCESS || res == BT::NodeStatus::FAILURE)
            {
                _game.getGlobalRegistry().remove<galaxy::Behavior>(entity);
            }
        }
        // delay deletions after the update
        _game.getDispatcher().update();
    };
};
} // namespace galaxy