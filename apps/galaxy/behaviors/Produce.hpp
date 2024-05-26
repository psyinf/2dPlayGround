#pragma once

#include <behaviors/BehaviorActionNode.hpp>

namespace behavior {

class Produce : public BehaviorActionNode
{
    using BehaviorActionNode::BehaviorActionNode;

public:
    static BT::PortsList providedPorts() { return {}; }

    BT::NodeStatus onStart() override
    {
        auto res = getInput<std::vector<entt::entity>>("targetsAvailable");

        return BT::NodeStatus::RUNNING;
    }

    BT::NodeStatus onRunning() override
    {
        // keep on waiting

        return BT::NodeStatus::SUCCESS;
    }
};

} // namespace behavior