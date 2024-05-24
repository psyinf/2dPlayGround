#pragma once

#include <behaviortree_cpp/action_node.h>
#include <pgGame/core/Game.hpp>

namespace behavior {

class BehaviorActionNode : public BT::StatefulActionNode
{
public:
    BehaviorActionNode(const std::string& name, pg::game::Game* game)
      : BT::StatefulActionNode(name, {})
      , game_ptr(game)
    {
    }

    BehaviorActionNode(const std::string& name)
      : BT::StatefulActionNode(name, {})
    {
    }

    void setEntity(entt::entity entity) { entity_ref = entity; }

    pg::game::Game& game()
    {
        if (!game_ptr) { throw std::runtime_error("Game is not set"); }
        return *game_ptr;
    }

    auto entity()
    {
        if (entity_ref == entt::null) { throw std::runtime_error("Entity is not set"); }
        return entity_ref;
    }

    void onHalted() override
    {
        // std::cout << "Halted " << registrationName() << entt::to_integral(entity()) << std::endl;
    }

private:
    pg::game::Game* game_ptr{nullptr};
    entt::entity    entity_ref{entt::null};
};

} // namespace behavior