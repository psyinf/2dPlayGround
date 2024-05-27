#pragma once

#include <behaviortree_cpp/action_node.h>
#include <pgGame/core/Game.hpp>

namespace behavior {

struct Context
{
    pg::game::Game*          game;
    BT::BehaviorTreeFactory& factory;
};

using ContextPtr = std::shared_ptr<Context>;

class BehaviorActionNode : public BT::StatefulActionNode
{
public:
    BehaviorActionNode(const std::string& name, const BT::NodeConfig& config, ContextPtr ctx)
      : BT::StatefulActionNode(name, config)
      , context(ctx)
    {
    }

    BehaviorActionNode(const std::string& name, ContextPtr ctx)
      : BT::StatefulActionNode(name, {})
      , context(ctx)
    {
    }

    BehaviorActionNode(const std::string& name)
      : BT::StatefulActionNode(name, {})
    {
    }

    static BT::PortsList providedPorts() { return {}; }

    void setEntity(entt::entity entity) { entity_ref = entity; }

    pg::game::Game& game()
    {
        if (!context) { throw std::runtime_error("Game is not set"); }
        return *(context->game);
    }

    auto entity()
    {
        if (entity_ref == entt::null) { throw std::runtime_error("Entity is not set"); }
        return entity_ref;
    }

    auto ctx() { return context; }

    void onHalted() override
    {
        // std::cout << "Halted " << registrationName() << entt::to_integral(entity()) << std::endl;
    }

    BT::NodeStatus onRunning() override { return BT::NodeStatus::SUCCESS; }

private:
    ContextPtr   context{nullptr};
    entt::entity entity_ref{entt::null};
};

} // namespace behavior