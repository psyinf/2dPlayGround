#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgGame/core/Game.hpp>
#include <pgEngine/math/Vec.hpp>
#include <pgEngine/math/Quadtree.hpp>
#include <pgEngine/math/Box.hpp>
#include <pgGame/entities/Drawable.hpp>
#include "events/PickEvent.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <deque>
#include <ranges>
#include <pgEngine/math/Transform.hpp>
#include <pgEngine/primitives/Sprite.hpp>

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
        if (lastPicks.empty()) { return; }
        const auto pick = lastPicks.back();

        auto& quadtree = game.getSingleton<const pg::Quadtree<entt::entity>&>("galaxy.quadtree");
        auto& marker = game.getSingleton<entt::entity>("galaxy.debug.marker");
        auto& transform = game.getRegistry().get<pg::Transform2D>(marker);
        auto  scaled_range = pg::fVec2{5, 5} * (1.0f / pick.scale);

        auto results = quadtree.rangeQuery(pg::fBox{pick.world_position - scaled_range, 2.0f * scaled_range});
        if (!results.empty())
        {
            transform.pos = results.at(0).box.midpoint();
            transform.scale = {0.025, 0.025};
        }
        else { transform.scale = {0, 0}; }

        lastPicks.clear();
    };

    void processPick(const galaxy::events::PickEvent& pickEvent) { lastPicks.push_back(pickEvent); }

private:
    std::deque<galaxy::events::PickEvent> lastPicks;
};
} // namespace galaxy
