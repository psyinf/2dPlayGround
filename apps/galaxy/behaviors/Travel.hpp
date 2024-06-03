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
    BT::NodeStatus onStart() { return BT::NodeStatus::RUNNING; }

    // You must override the virtual function tick()
    BT::NodeStatus onRunning() override
    {
        auto view = game().getRegistry().view<galaxy::Drone, pg::Transform2D, galaxy::Dynamic>();
        auto&& [drone, transform, dynamic] = view.get<galaxy::Drone, pg::Transform2D, galaxy::Dynamic>(entity());
        if (drone.targetId == entt::null)
        { //
            return BT::NodeStatus::FAILURE;
        }
        // travel to target
        // transform.pos += (drone.targetPos - transform.pos) * 0.01f;
        transform.pos = dynamic.calculateDynamics(
            pg::vec_cast<float>(transform.pos), drone.targetPos, drone.maxAcceleration, drone.maxVelocity, 0.02f);
        // check if we are at the target
        if (pg::lengthSquared(transform.pos - drone.targetPos) < 0.001f)
        {
            transform.pos = drone.targetPos;
            dynamic.reset();

            drone.waitCycles = 100;
            // TODO: event
            // mark the star as visited
            auto& starsystem = game().getRegistry().get<galaxy::StarSystemState>(drone.targetId);
            starsystem.colonizationStatus = galaxy::ColonizationStatus::Explored;
            return BT::NodeStatus::SUCCESS;
        }
        return BT::NodeStatus::RUNNING;
    }
};

} // namespace behavior