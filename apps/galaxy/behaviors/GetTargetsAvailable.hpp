#pragma once

#include <behaviors/BehaviorActionNode.hpp>
#include <behaviors/PortDefinitions.hpp>
#include <helpers/GalaxyHelpers.hpp>

namespace behavior {

class GetTargetsAvailable : public BehaviorActionNode
{
public:
    using BehaviorActionNode::BehaviorActionNode;

    static BT::PortsList providedPorts()
    {
        return {BT::OutputPort<BT::SharedQueue<uint64_t>>("available_target_list"), //
                BT::InputPort<uint8_t>("max_targets_to_find")};
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
        // TODO: assert result.data is always of size 1
        auto result_systems =
            quadtree.rangeQuery(pg::fBox::fromMidpoint(transform.pos, range)) | std::views::filter(filterOutOwnSystem) |
            std::views::filter(onlyUnexplored) |
            // take only .data member of the result
            std::views::transform([](auto result) { return entt::to_integral(result.data.front()); }) |
            std::ranges::to<std::deque<uint64_t>>();
        //clang-format on
        if (result_systems.empty()) { return BT::NodeStatus::FAILURE; }
        else
        {
            auto res = result_systems | std::views::take(getInput<size_t>("max_targets_to_find").value());
            auto shared_queue = std::make_shared<std::deque<uint64_t>>(res.begin(), res.end());
            setOutput("available_target_list", shared_queue);
            return BT::NodeStatus::SUCCESS;
        }
    }
};

} // namespace behavior