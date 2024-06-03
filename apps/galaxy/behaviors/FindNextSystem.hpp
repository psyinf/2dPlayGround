#pragma once

#include <behaviors/utils/BehaviorActionNode.hpp>

namespace behavior {

class FindNextSystem : public BehaviorActionNode
{
    using BehaviorActionNode::BehaviorActionNode;

public:
    BT::NodeStatus onStart() override
    {
        auto& quadtree = game().getSingleton<const pg::Quadtree<entt::entity>&>("galaxy.quadtree");
        auto  view = game().getRegistry().view<galaxy::Drone, pg::Transform2D, galaxy::Faction>();
        auto&& [drone, transform, faction] = view.get<galaxy::Drone, pg::Transform2D, galaxy::Faction>(entity());

        auto r =
            quadtree.rangeQuery(pg::fBox::fromMidpoint(transform.pos, {drone.range * 0.5f, drone.range * 0.5f})) |
            std::views::filter([&transform](auto result) { return !equal(result.box.midpoint(), transform.pos); }) |
            // remove stars that are already visited
            std::views::filter([this](auto result) {
                auto& starsystem = game().getRegistry().get<galaxy::StarSystemState>(result.data.front());
                return starsystem.colonizationStatus == galaxy::ColonizationStatus::Unexplored;
            });
        // to vector
        auto res = r | std::ranges::to<std::vector<pg::Quadtree<entt::entity>::Result>>();
        if (!res.empty())
        {
            // pick an index
            auto index = pg::randomBetween(static_cast<size_t>(0), res.size() - 1);
            drone.targetPos = res[index].box.midpoint();

            drone.targetId = res[index].data.front();
            auto&& [starsystem, sys_faction] =
                game().getRegistry().get<galaxy::StarSystemState, galaxy::Faction>(drone.targetId);
            sys_faction = faction;
            starsystem.colonizationStatus = galaxy::ColonizationStatus::Planned;
            return BT::NodeStatus::SUCCESS;
        }
        return BT::NodeStatus::FAILURE;
    }

    BT::NodeStatus onRunning() override
    {
        // should not be called
        return BT::NodeStatus::SUCCESS;
    }
};

} // namespace behavior