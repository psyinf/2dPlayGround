#include "RenderSystem.hpp"
#include "core/Game.h"
#include <entities/Entities.h>
#include <numbers>

void game::RenderSystem::setup() {}

class ScopedColor
{
public:
    ScopedColor(sdl::Renderer& r, const pg::Color& setColor)
      : renderer(r)
    {
        renderer.getDrawColor(&color[0], &color[1], &color[2], &color[3]);
        renderer.setDrawColor(setColor[0], setColor[1], setColor[2], setColor[3]);
    }

    ~ScopedColor() { renderer.setDrawColor(color[0], color[1], color[2], color[3]); }

private:
    sdl::Renderer& renderer;
    pg::Color          color;
};

// TODO: draw to texture and scale
static void renderSDL(sdl::Renderer& renderer, const pg::BoundingSphere& bs, const pg::Transform& transform)
{
    ScopedColor            sc(renderer, {0xff, 0xff, 0xff, 0xff});
    std::vector<pg::iVec2> circle_points;
    float                  radius = bs.radius * *std::ranges::max_element(transform.scale);
    for (int i = 0; i < 360; i++)
    {
        auto p = transform.pos + pg::fVec2{
                                     radius * std::cosf(i * std::numbers::pi / 180), //
                                     radius * std::sinf(i * std::numbers::pi / 180)  //
                                 };

        circle_points.push_back(vec_cast<int>(p));
    }
    renderer.drawLines(std::bit_cast<SDL_Point*>(circle_points.data()), circle_points.size());
}

void game::RenderSystem::handle(const FrameStamp& frameStamp)
{
    auto& renderer = game.getApp().getRenderer();
    renderer.clear();

    for (auto view = game.getRegistry().view<game::Drawable, pg::Transform>(); auto& entity : view)
    {
        auto&& [drawable, transform] = view.get<game::Drawable, pg::Transform>(entity);
        drawable.prim->draw(renderer, transform);
    }
    // TODO: render debug primitives like collision shapes

    for (auto boundsView = game.getRegistry().view<pg::BoundingSphere, pg::Transform>(); auto& entity : boundsView)
    {
        auto&& [bound, transform] = boundsView.get<pg::BoundingSphere, pg::Transform>(entity);
        renderSDL(renderer, bound, transform);
    }
    renderer.present();
}
