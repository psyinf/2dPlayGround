#pragma once
#include <behaviortree_cpp/decorators/loop_node.h>

namespace behavior {

class ProductionQueueLoop : public BT::LoopNode<entt::entity>
{
public:
    ProductionQueueLoop(const std::string& name, const BT::NodeConfig& cfg, ContextPtr ctx)
      : BT::LoopNode<entt::entity>(name, ctx->getPortInjector(name).injectPorts(cfg))

    {
    }

    ProductionQueueLoop(const std::string& name)
      : BT::LoopNode<entt::entity>(name, {})
    {
    }
};

} // namespace behavior