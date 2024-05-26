#pragma once

#include <behaviortree_cpp/action_node.h>
#include <pgGame/core/Game.hpp>

namespace behavior {

struct Context
{
    pg::game::Game&          game;
    BT::BehaviorTreeFactory& factory;
};

class BehaviorActionNode : public BT::StatefulActionNode
{
public:
    BehaviorActionNode(const std::string& name, std::shared_ptr<Context>& ctx)
      : BT::StatefulActionNode(name, {})
      , context(ctx)
    {
    }

    BehaviorActionNode(const std::string& name, const BT::NodeConfig& config, std::shared_ptr<Context>& ctx)
      : BT::StatefulActionNode(name, config)
      , context(ctx)
    {
    }

    BehaviorActionNode(const std::string& name, const BT::NodeConfig& config)
      : BT::StatefulActionNode(name, config)

    {
    }

    BehaviorActionNode(const std::string& name)
      : BT::StatefulActionNode(name, {})
    {
    }

    void setEntity(entt::entity entity) { entity_ref = entity; }

    pg::game::Game& game() { return context->game; }

    auto entity()
    {
        if (entity_ref == entt::null) { throw std::runtime_error("Entity is not set"); }
        return entity_ref;
    }

    void onHalted() override
    {
        // std::cout << "Halted " << registrationName() << entt::to_integral(entity()) << std::endl;
    }

    BT::NodeStatus onRunning() override
    {
        // should not be called
        return BT::NodeStatus::SUCCESS;
    }

    auto& ctx() { return *context.get(); }

private:
    std::shared_ptr<Context> context{};
    entt::entity             entity_ref{entt::null};
};

} // namespace behavior