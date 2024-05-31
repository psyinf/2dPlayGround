#include "Context.hpp"

#include <behaviors/utils/BehaviorActionNode.hpp>

behavior::PortInjector& behavior::Context::getPortInjector(const std::string& name)
{
    if (injectors.find(name) == injectors.end()) { injectors[name] = PortInjector(); }
    return injectors[name];
}

BT::Tree behavior::Context::setupTree(const std::string& name, entt::entity entity, BT::Blackboard::Ptr blackboard)
{
    auto behavior_tree = factory().createTree(name, blackboard);

    auto visitor = [&entity](BT::TreeNode* node) {
        if (auto action_node = dynamic_cast<behavior::BehaviorActionNode*>(node)) { action_node->setEntity(entity); }
    };

    // Apply the visitor to ALL the nodes of the tree
    behavior_tree.applyVisitor(visitor);

    return behavior_tree;
}
