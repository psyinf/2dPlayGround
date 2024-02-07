#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Vec.hpp>
#include "events/PickEvent.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <deque>
#include <ranges>

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
        std::ranges::for_each(lastPicks, [](const auto& pick) {
            fmt::print("PickingSystem::processPick {}\n", fmt::join(pick.screen_position, ","));
        });
        lastPicks.clear();
    };

    void processPick(const galaxy::events::PickEvent& pickEvent) { lastPicks.push_back(pickEvent); }

private:
    std::deque<galaxy::events::PickEvent> lastPicks;
};
} // namespace galaxy
