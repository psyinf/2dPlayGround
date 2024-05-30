#pragma once
#include <behaviortree_cpp/decorators/loop_node.h>

namespace behavior {

class TestLoop : public BT::LoopNode<entt::entity>
{
public:
    static auto makeNodeConfiguration(BT::NodeConfig cfg)
    {
        cfg.input_ports.insert({"queue", "{available_target_list}"});
        return cfg;
    }

    TestLoop(const std::string& name, const BT::NodeConfig& cfg)
      : BT::LoopNode<entt::entity>(name, makeNodeConfiguration(cfg))

    {
    }

    TestLoop(const std::string& name)
      : BT::LoopNode<entt::entity>(name, {})
    {
    }
};

} // namespace behavior