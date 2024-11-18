#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgGame/core/Game.hpp>
#include <pgEngine/math/Vec.hpp>
#include <pgEngine/math/Quadtree.hpp>
#include <pgEngine/math/Box.hpp>
#include <pgGame/components/Drawable.hpp>
#include "events/PickEvent.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <deque>
#include <ranges>
#include <pgEngine/math/Transform.hpp>
#include <pgEngine/primitives/Sprite.hpp>

#include <components/PickedEntity.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class PickingSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view /*scene_id*/) override
    {
        _game.getDispatcher().sink<galaxy::events::PickEvent>().connect<&galaxy::PickingSystem::processPick>(*this);
        // TODO: build a quadtree for picking
    };

    void handle(const pg::game::FrameStamp&) override
    {
        if (lastPicks.empty()) { return; }
        const auto pick = lastPicks.back();

        auto& quadtree = _game.getCurrentScene().getSingleton<const pg::Quadtree<entt::entity>&>("galaxy.quadtree");
        auto& marker = _game.getCurrentScene().getSingleton<entt::entity>("galaxy.debug.marker");
        auto& transform = _game.getGlobalRegistry().get<pg::Transform2D>(marker);
        auto  scaled_range = pg::fVec2{5, 5} * (1.0f / pick.scale);

        auto results = quadtree.rangeQuery(pg::fBox{pick.world_position - scaled_range, 2.0f * scaled_range});
        if (!results.empty())
        {
            transform.pos = results.at(0).box.midpoint();
            transform.scale = {0.025f, 0.025f};

            _game.getOrCreateSingleton<PickedEntity>("picked.entity").entity = results.at(0).data.at(0);

            auto event = galaxy::events::PickResult{.world_position{transform.pos}, .entity{results.at(0).data[0]}};
            _game.getDispatcher().trigger(event);
        }
        else { transform.scale = {0, 0}; }

        lastPicks.clear();
    };

    void processPick(const galaxy::events::PickEvent& pickEvent) { lastPicks.push_back(pickEvent); }

private:
    std::deque<galaxy::events::PickEvent> lastPicks;
};
} // namespace galaxy
