#pragma once

#include <behaviors/BehaviorActionNode.hpp>

namespace behavior {

class BuildDrone : public BehaviorActionNode
{
    using BehaviorActionNode::BehaviorActionNode;

public:
    BT::NodeStatus onStart() override { return BT::NodeStatus::RUNNING; }

    BT::NodeStatus onRunning() override
    {
        // keep on waiting

        return BT::NodeStatus::SUCCESS;
    }
};

} // namespace behavior