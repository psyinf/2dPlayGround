#include "RenderSystem.hpp"
#include <entities/Entities.h>
#include "core/Game.h"
void game::RenderSystem::setup() {}

void game::RenderSystem::handle(const FrameStamp& frameStamp)
{
    auto& renderer = game.getApp().getRenderer();
    renderer.clear();

    auto view = game.getRegistry().view<game::Drawable, pg::Transform>();
    for (auto& entity : view)
    {
        auto&& [drawable, transform] = view.get<game::Drawable, pg::Transform>(entity);
        drawable.prim->draw(renderer, transform);
    }
    renderer.present();
}
