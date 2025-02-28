#pragma once

#include <behaviortree_cpp/action_node.h>
#include <pgEngine/math/Transform.hpp>
#include <components/Drone.hpp>
#include <pgEngine/math/Vec.hpp>

namespace behavior {

class Travel : public BehaviorActionNode
{
    using BehaviorActionNode::BehaviorActionNode;

public:
    BT::NodeStatus onStart() override { return BT::NodeStatus::RUNNING; }

    // You must override the virtual function tick()
    BT::NodeStatus onRunning() override
    {
        auto view = game().getGlobalRegistry().view<galaxy::Drone, pg::Transform2D, galaxy::Dynamic>();
        auto&& [drone, transform, dynamic] = view.get<galaxy::Drone, pg::Transform2D, galaxy::Dynamic>(entity());
        if (drone.targetId == entt::null)
        { //
            return BT::NodeStatus::FAILURE;
        }
        auto dt = 0.02 * game().getCurrentTimeStamp().getRealTimePassed_sec();
        transform.pos = dynamic.calculateDynamics(
            pg::vec_cast<float>(transform.pos), drone.targetPos, drone.maxAcceleration, drone.maxVelocity, dt);
        // check if we are at the target
        if (pg::lengthSquared(transform.pos - drone.targetPos) < (0.001f * game().getCurrentGameState().timeScale))
        {
            transform.pos = drone.targetPos;
            dynamic.reset();

            drone.waitCycles = 100;
            // TODO: event
            // mark the star as visited
            auto& starsystem = game().getGlobalRegistry().get<galaxy::StarSystemState>(drone.targetId);
            starsystem.colonizationStatus = galaxy::ColonizationStatus::Explored;
            return BT::NodeStatus::SUCCESS;
        }
        return BT::NodeStatus::RUNNING;
    }
};

} // namespace behavior