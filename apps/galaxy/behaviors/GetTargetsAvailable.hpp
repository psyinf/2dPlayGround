#pragma once

#include <behaviors/utils/BehaviorActionNode.hpp>
#include <helpers/GalaxyHelpers.hpp>
#include <spdlog/spdlog.h>

namespace BT {

template <>
inline entt::entity convertFromString<entt::entity>(StringView str)
{
    return entt::entity(convertFromString<uint64_t>(str));
}

template <>
inline SharedQueue<entt::entity> convertFromString<SharedQueue<entt::entity>>(StringView str)
{
    auto                      parts = splitString(str, ';');
    SharedQueue<entt::entity> output = std::make_shared<std::deque<entt::entity>>();
    for (const StringView& part : parts)
    {
        output->push_back(convertFromString<entt::entity>(part));
    }
    return output;
}

} // namespace BT

namespace behavior {

class GetTargetsAvailable : public BehaviorActionNode
{
public:
    using BehaviorActionNode::BehaviorActionNode;

    static BT::PortsList providedPorts()
    {
        return {BT::OutputPort<BT::SharedQueue<entt::entity>>("available_target_list"), //
                BT::InputPort<uint8_t>("max_targets_to_find")};
    }

    BT::NodeStatus onStart() override
    {
        auto& quadtree = game().getSingleton<const pg::Quadtree<entt::entity>&>("galaxy.quadtree");
        auto  view = game().getRegistry().view<pg::Transform2D, galaxy::Faction>();
        auto&& [transform, faction] = view.get<pg::Transform2D, galaxy::Faction>(entity());

        auto filterOutOwnSystem = [&transform](auto result) {
            return !pg::equal(result.box.midpoint(), transform.pos);
        };
        auto onlyUnexplored = [this](auto result) {
            auto& starsystem = game().getRegistry().get<galaxy::StarSystemState>(result.data.front());
            return starsystem.colonizationStatus == galaxy::ColonizationStatus::Unexplored;
        };

        const auto& drone_conf = galaxy::getFactionConfig(game(), faction).droneParams;
        const auto  range = pg::fVec2{drone_conf.max_range * 0.5f, drone_conf.max_range * 0.5f};
        //clang-format off
        // TODO: assert result.data is always of size 1
        auto result_systems = quadtree.rangeQuery(pg::fBox::fromMidpoint(transform.pos, range)) |
                              std::views::filter(filterOutOwnSystem) | std::views::filter(onlyUnexplored) |
                              // take only .data member of the result
                              std::views::transform([](auto result) { return (result.data.front()); }) |
                              std::ranges::to<std::deque<entt::entity>>();
        //clang-format on
        if (result_systems.empty()) { return BT::NodeStatus::FAILURE; }
        else
        {
            std::string id = config().blackboard->get<std::string>("ID");
            std::cout << id << std::endl;
            auto max_to_find_opt = getInput<size_t>("max_targets_to_find");
            // TODO: refactor into a generic approach
            size_t max_to_find = 0;
            if (max_to_find_opt.has_value()) { max_to_find = max_to_find_opt.value(); }
            else
            {
                spdlog::warn("max_targets_to_find not set in {}({}), using default value of 0", id, fullPath());
                max_to_find = 0;
            };

            auto res = result_systems | std::views::take(max_to_find);
            auto shared_queue = std::make_shared<std::deque<entt::entity>>(res.begin(), res.end());
            setOutput("available_target_list", shared_queue);
            return BT::NodeStatus::SUCCESS;
        }
    }
};

} // namespace behavior