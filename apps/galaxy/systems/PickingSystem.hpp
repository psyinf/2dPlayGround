#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Vec.hpp>
#include <pgEngine/math/Quadtree.hpp>
#include <pgEngine/math/Box.hpp>
#include "events/PickEvent.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <deque>
#include <ranges>
#include <pgEngine/math/Transform.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class PickingSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup() override
    {
        game.getDispatcher().sink<galaxy::events::PickEvent>().connect<&galaxy::PickingSystem::processPick>(*this);
        // TODO: build a quadtree for picking
    };

    void handle(const pg::game::FrameStamp& frameStamp) override
    {
        auto& quadtree = game.getSingleton<const pg::Quadtree&>("galaxy.quadtree");
        std::ranges::for_each(lastPicks, [&](const auto& pick) {
            fmt::print("PickingSystem::processPick {} {}\n",
                       fmt::join(pick.screen_position, ","),
                       fmt::join(pick.world_position, ","));
            auto marker = game.getSingleton<entt::entity>("galaxy.debug.marker");
            game.getRegistry().get<pg::Transform2D>(marker).pos = {pick.world_position};
        });
        lastPicks.clear();
    };

    void processPick(const galaxy::events::PickEvent& pickEvent) { lastPicks.push_back(pickEvent); }

private:
    std::deque<galaxy::events::PickEvent> lastPicks;
};
} // namespace galaxy
