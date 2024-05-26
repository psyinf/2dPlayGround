#pragma once

#include <behaviors/BehaviorActionNode.hpp>

namespace BT {
template <>
inline std::vector<entt::entity> convertFromString(StringView str)
{
    // We expect real numbers separated by semicolons
    auto                      parts = splitString(str, ';');
    std::vector<entt::entity> output;
    for (auto& part : parts)
    {
        output.push_back(convertFromString<entt::entity>(part));
    }

    return output;
}
} // end namespace BT

namespace behavior {

class GetTargetsAvailable : public BehaviorActionNode
{
public:
    using BehaviorActionNode::BehaviorActionNode;

    static BT::PortsList providedPorts() { return {BT::OutputPort<std::vector<entt::entity>>("targetsAvailable")}; }

    BT::NodeStatus onStart() override
    {
        auto& quadtree = game().getSingleton<const pg::Quadtree<entt::entity>&>("galaxy.quadtree");
        auto  view = game().getRegistry().view<galaxy::Drone, pg::Transform2D, galaxy::Faction>();
        auto&& [drone, transform, faction] = view.get<galaxy::Drone, pg::Transform2D, galaxy::Faction>(entity());

        auto result_systems =
            quadtree.rangeQuery(pg::fBox::fromMidpoint(transform.pos, {drone.range * 0.5f, drone.range * 0.5f})) |
            std::views::filter([&transform](auto result) { return !equal(result.box.midpoint(), transform.pos); }) |
            std::views::filter([this](auto result) {
                auto& starsystem = game().getRegistry().get<galaxy::StarSystemState>(result.data.front());
                return starsystem.colonizationStatus == galaxy::ColonizationStatus::Unexplored;
            }) |
            std::ranges::to<std::vector<pg::Quadtree<entt::entity>::Result>>();

        if (result_systems.empty()) { return BT::NodeStatus::FAILURE; }
        else
        {
            setOutput("targetsAvailable", result_systems);
            return BT::NodeStatus::SUCCESS;
        }
    }
};

} // namespace behavior