#include "RenderSystem.hpp"
#include <entities/StarSystem.hpp>
#include <pgGame/entities/Drawable.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/entities/WindowDetails.hpp>

void galaxy::RenderSystem::setup() {}

void galaxy::RenderSystem::handle(const pg::game::FrameStamp& frameStamp)
{
    auto& renderer = game.getApp().getRenderer();
    renderer.clear();
    auto rendererStates = pg::States{};
    // rendererStates.push(pg::TextureColorState{pg::Color{255, 255, 0, 255}});
    rendererStates.push(pg::TextureBlendModeState{SDL_BLENDMODE_ADD});
    rendererStates.apply(renderer);

    auto globalTransform = game.getSingleton<pg::Transform2D>(pg::game::Scene::GlobalTransformName);

    for (auto view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D>(); auto& entity : view)
    {
        auto&& [drawable, transform] = view.get<pg::game::Drawable, pg::Transform2D>(entity);
        drawable.prim->draw(renderer, transform + globalTransform, rendererStates);
    }
    rendererStates.restore(renderer);
    renderer.present();
}
