#pragma once
#include <behaviortree_cpp/action_node.h>
#include <behaviortree_cpp/bt_factory.h>

#include <pgGame/core/Game.hpp>
#include <helpers/GalaxyHelpers.hpp>

namespace behavior {
// NOT YET USED
class BehaviorTreeSetup
{
public:
    BehaviorTreeSetup(pg::game::Game& game)
      : game(game)
    {
    }

    BT::BehaviorTreeFactory factory;
    pg::game::Game&         game;
};

} // namespace behavior