#include "RenderSystem.hpp"
#include <entities/StarSystem.hpp>
#include <pgGame/entities/Drawable.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/entities/WindowDetails.hpp>
#include <pgEngine/math/VecUtils.hpp>
#include <entities/Tags.hpp>
#include <entities/Drone.hpp>
#include <entities/Faction.hpp>
#include <pgGame/entities/RenderState.hpp>

void galaxy::RenderSystem::setup() {}

/// TODO: drawables should carry their respective states. This is a temporary solution which determines the state by the
/// type of entities

void galaxy::RenderSystem::handle(const pg::game::FrameStamp& frameStamp)
{
    auto& renderer = game.getApp().getRenderer();
    renderer.clear();
    auto rendererStates = pg::States{};
    // rendererStates.push(pg::TextureColorState{pg::Color{255, 255, 0, 255}});
    rendererStates.push(pg::TextureBlendModeState{SDL_BLENDMODE_ADD});
    rendererStates.apply(renderer);
    auto windowRect = game.getSingleton<pg::game::WindowDetails>().windowRect;
    auto star_default_color = game.getSingleton<const pg::Color&>("galaxy.star.default_color");

    auto globalTransform = game.getSingleton<pg::Transform2D>(pg::game::Scene::GlobalTransformName);
    // star systems

    for (auto view =
             game.getRegistry().view<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState, galaxy::Faction>();
         auto& entity : view)
    {
        continue;

        auto&& [drawable, transform, systemState, faction] =
            view.get<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState, galaxy::Faction>(entity);
        // TODO: make this a camera class and use homogeneous matrices
        auto new_transform = transform;
        new_transform.pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
        new_transform.pos = (globalTransform.pos + transform.pos) * globalTransform.scale;
        new_transform.pos += pg::dimsFromRect<float>(windowRect) * 0.5f;
        switch (systemState.colonizationStatus)
        {
        case galaxy::ColonizationStatus::Explored:
            rendererStates.push(pg::TextureColorState{faction.entityColor});
            drawable.prim->draw(renderer, new_transform, rendererStates);
            rendererStates.pop<pg::TextureColorState>();
            break;

        default:
            rendererStates.push(pg::TextureColorState{star_default_color});
            drawable.prim->draw(renderer, new_transform, rendererStates);
            rendererStates.pop<pg::TextureColorState>();
            break;
        }
    }

    // drones
    auto view = game.getRegistry().group<pg::game::Drawable, pg::Transform2D, galaxy::Drone, galaxy::Faction>();
    view.sort<galaxy::Faction>(
        [](const galaxy::Faction& lhs, const galaxy::Faction& rhs) { return lhs.name < rhs.name; });

    for (auto& entity : view)
    {
        auto&& [drawable, transform, drone, faction] =
            view.get<pg::game::Drawable, pg::Transform2D, galaxy::Drone, galaxy::Faction>(entity);

        auto new_transform = transform;
        new_transform.pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
        new_transform.pos = (globalTransform.pos + transform.pos) * globalTransform.scale;
        new_transform.pos += pg::dimsFromRect<float>(windowRect) * 0.5f;
        new_transform.scale *= globalTransform.scale;

        rendererStates.push(pg::TextureColorState{faction.entityColor});
        drawable.prim->draw(renderer, new_transform, rendererStates);
        rendererStates.pop<pg::TextureColorState>();
    }

    // non-debug, generic items

    for (auto view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D, pg::game::RenderState>(
             entt::exclude<pg::tags::DebugRenderingItemTag>);
         auto& entity : view)
    {
        auto&& [drawable, transform, renderState] =
            view.get<pg::game::Drawable, pg::Transform2D, pg::game::RenderState>(entity);

        rendererStates.push_states(renderState.states);
        auto new_transform = transform;
        new_transform.pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
        new_transform.pos = (globalTransform.pos + transform.pos) * globalTransform.scale;
        new_transform.pos += pg::dimsFromRect<float>(windowRect) * 0.5f;
        // TODO: scale globally should be a flag
        new_transform.scale *= globalTransform.scale;
        drawable.prim->draw(renderer, new_transform, rendererStates);
        rendererStates.pop_states(renderState.states);
    }
    // debug items, TODO: check why this not drawn reliably over other items
    if (game.getSingleton<const bool>("galaxy.debug.draw"))
    {
        for (auto view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D, pg::tags::DebugRenderingItemTag>(
                 entt::exclude<StarSystemState>);
             auto& entity : view)
        {
            auto&& [drawable, transform] = view.get<pg::game::Drawable, pg::Transform2D>(entity);
            auto new_transform = transform;
            new_transform.pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
            new_transform.pos = (globalTransform.pos + transform.pos) * globalTransform.scale;
            new_transform.pos += pg::dimsFromRect<float>(windowRect) * 0.5f;
            new_transform.scale *= globalTransform.scale;
            drawable.prim->draw(renderer, new_transform, rendererStates);
        }
    }

    rendererStates.restore(renderer);
    renderer.present();
}
