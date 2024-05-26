#pragma once

#include <behaviors/TreeSetupHelper.hpp>

#include <pgEngine/math/Transform.hpp>
#include <components/Drone.hpp>
#include <pgEngine/math/Vec.hpp>

#include <helpers/GalaxyHelpers.hpp>
#include <behaviors/BehaviorActionNode.hpp>

namespace behavior {

class MakeDrone : public BehaviorActionNode
{
    using BehaviorActionNode::BehaviorActionNode;

public:
    // You must override the virtual function tick()
    BT::NodeStatus onStart() override
    {
        auto& game = this->game();
        auto  maker_entity = entity();
        auto  view = game.getRegistry().view<pg::Transform2D, galaxy::Faction>();

        auto&& [transform, faction] = view.get<pg::Transform2D, galaxy::Faction>(maker_entity);

        auto        dot_sprite = game.getTypedResourceCache<pg::Sprite>().load("../data/circle_05.png");
        const auto& drone_params = galaxy::getFactionConfig(game, faction).droneParams;

        // create a drone
        auto renderState = pg::States{};
        renderState.push(pg::TextureColorState{faction.entityColor});
        auto f = faction;
        auto entity = //
            pg::game::makeEntity<pg::Transform2D,
                                 pg::game::Drawable,
                                 galaxy::Drone,
                                 galaxy::Dynamic,
                                 galaxy::Faction,
                                 galaxy::Lifetime,
                                 pg::game::RenderState>(game.getRegistry(),
                                                        {.pos{transform.pos}, .scale{0.00125, 0.00125}},
                                                        pg::game::Drawable{dot_sprite},
                                                        {galaxy::Drone::fromConfig(drone_params)},
                                                        galaxy::Dynamic{},
                                                        std::move(f),
                                                        galaxy::Lifetime{},
                                                        {std::move(renderState)});

        // add behavior"
        auto behavior_tree = setupTree(ctx()->factory, "Drone", entity);
        pg::game::addComponent<galaxy::Behavior>(
            game.getRegistry(), entity, galaxy::Behavior{std::move(behavior_tree)});

        game.getDispatcher().trigger<galaxy::events::DroneCreatedEvent>({entity, transform.pos});
        return BT::NodeStatus::SUCCESS;
    }
};

} // namespace behavior