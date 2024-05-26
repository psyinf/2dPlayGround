#pragma once
#include <behaviortree_cpp/action_node.h>

namespace behavior {

BT::Tree setupTree(BT::BehaviorTreeFactory& factory, const std::string& name, entt::entity entity)
{
    auto behavior_tree = factory.createTree(name);

    auto visitor = [&entity](BT::TreeNode* node) {
        if (auto action_node = dynamic_cast<behavior::BehaviorActionNode*>(node)) { action_node->setEntity(entity); }
    };

    // Apply the visitor to ALL the nodes of the tree
    behavior_tree.applyVisitor(visitor);
    behavior_tree.rootBlackboard()->set("entity", entity);
    return behavior_tree;
}

} // namespace behavior