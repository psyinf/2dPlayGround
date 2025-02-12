#pragma once
#include <pgGame/systems/SystemInterface.hpp>
#include <components/Behavior.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class BehaviorSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view /*scene_id*/) { auto view = _game.getGlobalRegistry().view<galaxy::Behavior>(); }

    void handle(const pg::FrameStamp&)
    {
        auto view = _game.getGlobalRegistry().view<galaxy::Behavior>();

        for (auto entity : view)
        {
            auto& behavior = view.get<galaxy::Behavior>(entity);

            static bool once = false;
            if (!once)
            {
                once = true;
                behavior.publisher = std::make_shared<BT::Groot2Publisher>(behavior.tree);
            }
            auto res = behavior.tree.tickExactlyOnce();

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