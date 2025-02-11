#pragma once
#include <entt/entt.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Vec.hpp>
#include <pgGame/components/Drawable.hpp>
#include <components/Faction.hpp>
#include <components/StarSystem.hpp>
#include <components/Orbit.hpp>

#include <events/DroneEvents.hpp>
#include <pgGame/components/RenderState.hpp>

#include <Config.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <components/Tags.hpp>
#include <events/SystemEvents.hpp>
#include <Galaxy.hpp>
#include <helpers/GalaxyHelpers.hpp>
#include <pgGame/components/RenderState.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class UpdateStarsSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view /*scene_id*/) override
    {
        _game.getDispatcher().sink<galaxy::events::DroneCreatedEvent>().connect<&UpdateStarsSystem::handleDroneCreated>(
            this);
        _game.getDispatcher()
            .sink<galaxy::events::SystemOwnerChangedEvent>()
            .connect<&UpdateStarsSystem::handleSystemOwnerChanged>(this);
    };

    void handleDroneCreated(galaxy::events::DroneCreatedEvent& event)
    {
        // TODO: use an animation

        auto& transform = _game.getGlobalRegistry().get<pg::Transform2D>(event.entity);
        auto& marker_transform = _game.getCurrentSceneRegistry().get<pg::Transform2D>(
            _game.getCurrentScene().getSingleton<entt::entity>("galaxy.debug.marker"));
        marker_transform.pos = transform.pos;
    }

    void handleSystemOwnerChanged(galaxy::events::SystemOwnerChangedEvent& event)
    {
        // TODO: get the star system's renderstate

        auto&& [starSystemState, renderState] =
            _game.getGlobalRegistry().get<galaxy::StarSystemState, pg::game::RenderState>(event.system_entity);
        // get faction config color
        auto faction_config = galaxy::getFactionConfig(_game, event.owner_faction);

        switch (starSystemState.colonizationStatus)
        {
        case galaxy::ColonizationStatus::Explored:
            renderState.states.replace<pg::TextureColorState>({faction_config.color});
            break;
        case galaxy::ColonizationStatus::Planned:
            renderState.states.replace<pg::TextureColorState>({pg::scale(faction_config.color, 0.2f)});
            break;
        default:
            renderState.states.replace<pg::TextureColorState>(
                {pg::asRBGA(pgOrbit::getForSpectralType(pgOrbit::StarColors, starSystemState.spectralType))});
            break;
        }
    }

    void handle(const pg::FrameStamp& frameStamp) override
    {
        updateStarSystems(frameStamp);
        // move the stars on a circle at the distance from center
        //             auto distance = normalize(transform.pos);
        //             auto angle = atan2(transform.pos[1], transform.pos[0]);
        //             angle += 0.0001 * sqrt(1.0 / distance);
        //             transform.pos = {distance * cos(angle), distance * sin(angle)};
        //
    }

    void updateStarSystems(const pg::FrameStamp&)
    {
        // get config from singleton
        // auto& galaxyConfig = _game.getCurrentScene().getSingleton<const
        // galaxy::config::Galaxy&>("galaxy.config");

        auto& registry = _game.getGlobalRegistry();
        auto  view = registry.view<pg::game::Drawable,
                                   pg::Transform2D,
                                   galaxy::StarSystemState,
                                   pg::game::RenderState,
                                   galaxy::Faction>();
        for (auto& entity : view)
        {
            auto&& [drawable, transform, starSystemState, state, faction] = view.get<pg::game::Drawable,
                                                                                     pg::Transform2D,
                                                                                     galaxy::StarSystemState,
                                                                                     pg::game::RenderState,
                                                                                     galaxy::Faction>(entity);

            // TODO: this needs to be controlable
            // e.g. the colonized view colors are dependent on the own factions knowledge
            //  and the view mode of the map
            // TODO: move to event handler
            //             state.states = std::move(pg::States{});
            //             switch (starSystemState.colonizationStatus)
            //             {
            //             case galaxy::ColonizationStatus::Explored:
            //
            //                 state.states.push(pg::TextureColorState{faction.entityColor});
            //                 break;
            //             case galaxy::ColonizationStatus::Planned:
            //                 state.states.push(pg::TextureColorState{pg::scale(faction.entityColor, 0.2f)});
            //                 break;
            //             default:
            //                 state.states.push(pg::TextureColorState{
            //                     pg::asRBGA(pgOrbit::getForSpectralType(pgOrbit::StarColors,
            //                     starSystemState.spectralType))});
            //                 break;
            //             }
        }
    }
};
} // namespace galaxy