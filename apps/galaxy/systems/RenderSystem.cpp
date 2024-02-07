#include "RenderSystem.hpp"
#include <entities/StarSystem.hpp>
#include <pgGame/entities/Drawable.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/entities/WindowDetails.hpp>
#include <pgEngine/math/VecUtils.hpp>

void galaxy::RenderSystem::setup() {}

void galaxy::RenderSystem::handle(const pg::game::FrameStamp& frameStamp)
{
    auto& renderer = game.getApp().getRenderer();
    renderer.clear();
    auto rendererStates = pg::States{};
    // rendererStates.push(pg::TextureColorState{pg::Color{255, 255, 0, 255}});
    rendererStates.push(pg::TextureBlendModeState{SDL_BLENDMODE_ADD});
    rendererStates.apply(renderer);
    auto windowRect = game.getSingleton<pg::game::WindowDetails>().windowRect;
    auto globalTransform = game.getSingleton<pg::Transform2D>(pg::game::Scene::GlobalTransformName);
    // star systems
    for (auto  view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState>();
         auto& entity : view)
    {
        auto&& [drawable, transform, systemState] =
            view.get<pg::game::Drawable, pg::Transform2D, galaxy::StarSystemState>(entity);
        // TODO: make this a camera class and use homogeneous matrices
        auto new_transform = transform;
        new_transform.pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
        new_transform.pos = (globalTransform.pos + transform.pos) * globalTransform.scale;
        new_transform.pos += pg::dimsFromRect<float>(windowRect) * 0.5f;
        switch (systemState.colonizationStatus)
        {
        case galaxy::ColonizationStatus::Colonized:
            rendererStates.push(pg::TextureColorState{pg::Color{255, 255, 0, 255}});
            drawable.prim->draw(renderer, new_transform, rendererStates);
            rendererStates.pop<pg::TextureColorState>();
            break;

        default:
            drawable.prim->draw(renderer, new_transform, rendererStates);
            break;
        }
    }
    rendererStates.push(pg::TextureAlphaState{128});
    for (auto  view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D>(entt::exclude<StarSystemState>);
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
    rendererStates.pop<pg::TextureAlphaState>();

    rendererStates.restore(renderer);
    renderer.present();
}
