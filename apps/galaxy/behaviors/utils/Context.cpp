#include "Context.hpp"

#include <behaviors/utils/BehaviorActionNode.hpp>

behavior::PortInjector& behavior::Context::getPortInjector(const std::string& name)
{
    if (injectors.find(name) == injectors.end()) { injectors[name] = PortInjector(); }
    return injectors[name];
}

void behavior::Context::setupBlackboard(BT::Blackboard::Ptr blackboard, entt::entity entity)
{
    // TODO: check if this is needed.
    return;
    auto        view = game->getRegistry().view<galaxy::Faction>();
    auto&&      faction = view.get<galaxy::Faction>(entity);
    const auto& factionConfig = galaxy::getFactionConfig(*game, faction);

    blackboard->set("entity", entity);
    blackboard->set("max_targets_to_find", factionConfig.startParams.start_drones);
}

BT::Tree behavior::Context::setupTree(const std::string& name, entt::entity entity)
{
    BT::Blackboard::Ptr blackboard = BT::Blackboard::create();
    setupBlackboard(blackboard, entity);
    auto behavior_tree = factory().createTree(name, blackboard);

    auto visitor = [&entity](BT::TreeNode* node) {
        if (auto action_node = dynamic_cast<behavior::BehaviorActionNode*>(node)) { action_node->setEntity(entity); }
    };

    // Apply the visitor to ALL the nodes of the tree
    behavior_tree.applyVisitor(visitor);

    return behavior_tree;
}
