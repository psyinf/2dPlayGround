#pragma once

#include <behaviortree_cpp/action_node.h>

namespace behavior {

class FindNextSystem : public BT::SyncActionNode
{
public:
    FindNextSystem(const std::string& name, pg::game::Game& game)
      : BT::SyncActionNode(name, {})
      , game(&game)
    {
    }

    FindNextSystem(const std::string& name)
      : BT::SyncActionNode(name, {})
      , game()
    {
    }

    // You must override the virtual function tick()
    BT::NodeStatus tick() override
    {
        std::cout << "ApproachObject: " << this->name() << std::endl;
        return BT::NodeStatus::SUCCESS;
    }

private:
    pg::game::Game* game{nullptr};
};

} // namespace behavior