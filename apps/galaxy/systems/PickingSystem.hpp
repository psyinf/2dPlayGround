#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
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
        auto& quadtree = game.getSingleton<const pg::Quadtree&>("galaxy.quadtree");
        std::ranges::for_each(
            lastPicks,
            [&](const auto& pick)

            {
                fmt::print("PickingSystem::processPick {} {}\n",
                           fmt::join(pick.screen_position, ","),
                           fmt::join(pick.world_position, ","));
                pg::DebugIntersectionCollector collector;
                auto                           results = quadtree.rangeQuery({pick.world_position, {5, 5}}, collector);
                if (!results.empty())
                {
                    fmt::print("PickingSystem::processPick found {} results\n", results.size());
                    auto marker = game.getSingleton<entt::entity>("galaxy.debug.marker");
                    auto&& [transform, drawable] = game.getRegistry().get<pg::Transform2D, pg::game::Drawable>(marker);
                    transform.pos = {results.at(0).midpoint()};
                    auto                         sprites = std::dynamic_pointer_cast<pg::Sprites>(drawable.prim);
                    std::vector<pg::Transform2D> transforms;
                    for (auto& result : results)
                    {
                        transforms.push_back({.pos{result.midpoint()}, .scale{0.005, 0.005}});
                    }
                    sprites->getTransforms() = transforms;
                    auto i = 0;
                    for (auto& node : collector.intersectedQuadTreeNodes)
                    {
                        auto cell = game.getSingleton<entt::entity>(fmt::format("galaxy.debug.cell_{}", i++));
                        auto&& [transform, drawable] =
                            game.getRegistry().get<pg::Transform2D, pg::game::Drawable>(cell);
                        drawable.prim = std::make_shared<pg::BoxPrimitive>(node, pg::Color{255, 0, 0, 255});
                    }
                }
                else
                {
                    auto i = 0;
                    for (auto i = 0; i < 4; ++i)
                    {
                        auto cell = game.getSingleton<entt::entity>(fmt::format("galaxy.debug.cell_{}", i));
                        auto&& [transform, drawable] =
                            game.getRegistry().get<pg::Transform2D, pg::game::Drawable>(cell);
                        drawable.prim = std::make_shared<pg::Placeholder>();
                    }
                }
            });

        lastPicks.clear();
    };

    void processPick(const galaxy::events::PickEvent& pickEvent) { lastPicks.push_back(pickEvent); }

private:
    std::deque<galaxy::events::PickEvent> lastPicks;
};
} // namespace galaxy
