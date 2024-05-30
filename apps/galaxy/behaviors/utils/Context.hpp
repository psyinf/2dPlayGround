#pragma once
#include <behaviors/utils/PortInjection.hpp>
#include <behaviortree_cpp/bt_factory.h>
#include <unordered_map>
#include <memory>
#include <helpers/GalaxyHelpers.hpp>

namespace behavior {
struct Context
{
    pg::game::Game*                               game;
    BT::BehaviorTreeFactory                       factory_;
    std::unordered_map<std::string, PortInjector> injectors;

    PortInjector& getPortInjector(const std::string& name);

    void setupBlackboard(BT::Blackboard::Ptr blackboard, entt::entity entity);

    // TODO: maybe pass a function for setting ip the blackboard
    BT::Tree setupTree(const std::string& name, entt::entity entity);

    auto factory() -> BT::BehaviorTreeFactory& { return factory_; }
};

using ContextPtr = std::shared_ptr<Context>;
} // namespace behavior