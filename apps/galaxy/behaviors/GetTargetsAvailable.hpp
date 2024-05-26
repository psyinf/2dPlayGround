#pragma once

#include <behaviors/BehaviorActionNode.hpp>
#include <helpers/GalaxyHelpers.hpp>

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

    static BT::PortsList providedPorts()
    {
        return {BT::OutputPort<std::vector<entt::entity>>("targetsAvailable"), //
                BT::InputPort<uint32_t>("findMax")};
    }

    BT::NodeStatus onStart() override
    {
        auto& quadtree = game().getSingleton<const pg::Quadtree<entt::entity>&>("galaxy.quadtree");
        auto  view = game().getRegistry().view<pg::Transform2D, galaxy::Faction>();
        auto&& [transform, faction] = view.get<pg::Transform2D, galaxy::Faction>(entity());

        auto filterOutOwnSystem = [&transform](auto result) { return !equal(result.box.midpoint(), transform.pos); };
        auto onlyUnexplored = [this](auto result) {
            auto& starsystem = game().getRegistry().get<galaxy::StarSystemState>(result.data.front());
            return starsystem.colonizationStatus == galaxy::ColonizationStatus::Unexplored;
        };

        const auto& drone_conf = galaxy::getFactionConfig(game(), faction).droneParams;
        const auto  range = pg::fVec2{drone_conf.max_range * 0.5f, drone_conf.max_range * 0.5f};
        //clang-format off
        auto result_systems = quadtree.rangeQuery(pg::fBox::fromMidpoint(transform.pos, range)) |
                              std::views::filter(filterOutOwnSystem) | std::views::filter(onlyUnexplored) |
                              std::ranges::to<std::vector<pg::Quadtree<entt::entity>::Result>>();
        //clang-format on
        if (result_systems.empty()) { return BT::NodeStatus::FAILURE; }
        else
        {
            auto res = result_systems | std::views::take(getInput<size_t>("findMax").value());

            setOutput("targetsAvailable", res);
            return BT::NodeStatus::SUCCESS;
        }
    }
};

} // namespace behavior