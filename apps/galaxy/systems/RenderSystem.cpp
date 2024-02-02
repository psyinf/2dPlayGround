#include "RenderSystem.hpp"
#include <entities/StarSystem.hpp>
#include <pgGame/entities/Drawable.hpp>
#include <pgGame/core/Game.hpp>

void galaxy::RenderSystem::setup() {}

void galaxy::RenderSystem::handle(const pg::game::FrameStamp& frameStamp)
{
    auto& renderer = game.getApp().getRenderer();
    renderer.clear();

    for (auto view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D>(); auto& entity : view)
    {
        auto&& [drawable, transform] = view.get<pg::game::Drawable, pg::Transform2D>(entity);
        drawable.prim->draw(renderer, transform, {});
    }
}
