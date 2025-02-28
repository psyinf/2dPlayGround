#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Transform.hpp>
#include <pgGame/components/Drawable.hpp>

namespace sdl {
class Renderer;
}

namespace pg {
class States;
}

namespace galaxy {
using entt::literals::operator""_hs;

struct RenderSystemConfig
{
    bool perScene = true;
    bool clear = true;
};

template <typename... Tags>
class TaggedRenderSystem : public pg::game::SystemInterface
{
public:
    TaggedRenderSystem(pg::game::Game& game, const std::string& name, const RenderSystemConfig& renderSystemConfig)
      : SystemInterface(game, name)
      , _renderSystemConfig(std::move(renderSystemConfig))
    {
    }

    void setup(std::string_view /*scene_id*/) override {}

    pg::Transform2D calculateTransform(const pg::Transform2D& transform,
                                       const pg::Transform2D& globalTransform,
                                       SDL_Rect               windowRect)
    {
        auto new_transform = transform;
        new_transform.pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
        new_transform.pos = (globalTransform.pos + transform.pos) * globalTransform.scale;
        new_transform.pos += pg::dimsFromRect<float>(windowRect) * 0.5f;
        if (transform.scaleSpace == pg::TransformScaleSpace::World) { new_transform.scale *= globalTransform.scale; }
        return new_transform;
    }

    void handle(const pg::FrameStamp& frameStamp) override
    {
        auto renderer = pg::Renderer{_game.getApp().getRenderer(), frameStamp};
        if (_renderSystemConfig.clear) { renderer.clear(); }
        // TODO: overarching renderer state in RenderSystemConfig
        auto rendererStates = pg::States{};
        rendererStates.apply(renderer.renderer);
        auto windowRect = _game.getCurrentScene().getSingleton<pg::game::WindowDetails>().windowRect;

        auto globalTransform =
            _game.getCurrentScene().getSingleton<pg::Transform2D>(pg::game::Scene::GlobalTransformName);

        // drones
        // non-debug, generic items

        for (auto view = getView<pg::game::Drawable, pg::game::RenderState, pg::Transform2D>(
                 entt::exclude<pg::tags::DebugRenderingItemTag>);
             auto& entity : view)
        {
            auto&& [drawable, transform, renderState] =
                view.template get<pg::game::Drawable, pg::Transform2D, pg::game::RenderState>(entity);

            rendererStates.push_states(renderState.states);

            drawable.prim->draw(renderer, calculateTransform(transform, globalTransform, windowRect), rendererStates);
            rendererStates.pop_states(renderState.states);
        }

        // local

        // TODO: global config for debug items

        // debug items, TODO: check why this not drawn reliably over other items
        //     if (game.getCurrentScene().getSingleton_<const
        //     galaxy::config::Galaxy&>("galaxy.config").debugging.draw_debug_items)
        //     {
        //         for (auto view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D,
        //         pg::tags::DebugRenderingItemTag>(
        //                  entt::exclude<StarSystemState>);
        //              auto& entity : view)
        //         {
        //             auto&& [drawable, transform] = view.get<pg::game::Drawable, pg::Transform2D>(entity);
        //             auto new_transform = transform;
        //             new_transform.pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
        //             new_transform.pos = (globalTransform.pos + transform.pos) * globalTransform.scale;
        //             new_transform.pos += pg::dimsFromRect<float>(windowRect) * 0.5f;
        //             new_transform.scale *= globalTransform.scale;
        //             drawable.prim->draw(renderer, new_transform, rendererStates);
        //         }
        //     }
        rendererStates.restore(renderer.renderer);
    }

    template <typename... Args, typename... Excludes>
    auto getView(Excludes... excludes)
    {
        if (_renderSystemConfig.perScene)
        {
            return _game.getCurrentSceneRegistry().view<Args..., Tags...>(std::forward<Excludes>(excludes)...);
        }
        else { return _game.getGlobalRegistry().view<Args..., Tags...>(std::forward<Excludes>(excludes)...); }
    }

private:
    RenderSystemConfig _renderSystemConfig;
};

using RenderSystem = TaggedRenderSystem<>;

} // namespace galaxy