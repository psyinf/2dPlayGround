#pragma once
#include <behaviors/BehaviorActionNode.hpp>
#include <helpers/GalaxyHelpers.hpp>

namespace behavior {

class SetFactionBlackboard : public BehaviorActionNode
{
public:
    using BehaviorActionNode::BehaviorActionNode;

    static BT::PortsList providedPorts() { return {BT::OutputPort<int8_t>("max_targets_to_find")}; }

    BT::NodeStatus onStart() override
    {
        auto   view = game().getRegistry().view<galaxy::Faction>();
        auto&& faction = view.get<galaxy::Faction>(entity());
        // get faction config
        const auto& factionConfig = galaxy::getFactionConfig(game(), faction);
        // we need to somehow remap
        setOutput("max_targets_to_find", factionConfig.startParams.start_drones);

        return BT::NodeStatus::SUCCESS;
    }
};
} // namespace behavior