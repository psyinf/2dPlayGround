#include "RenderSystem.hpp"
#include <pgEngine/math/Bounds.hpp>
#include <pgEngine/math/Transform.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/components/Drawable.hpp>
#include <components/Entities.h>
#include <numbers>
#include <cmath>

void asteroids::RenderSystem::setup(std::string_view /*scene_id*/) {}

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
    pg::Color      color;
};

// TODO: draw to texture and scale
static void renderSDL(sdl::Renderer& renderer, const pg::BoundingSphere& bs, const pg::Transform2D& transform)
{
    ScopedColor            sc(renderer, {0xff, 0xff, 0xff, 0xff});
    std::vector<pg::iVec2> circle_points;
    float                  radius = bs.radius * *std::ranges::max_element(transform.scale);
    for (int i = 0; i < 360; i++)
    {
        auto p = transform.pos + pg::fVec2{
                                     radius * cosf(static_cast<float>(i * std::numbers::pi) / 180.0f), //
                                     radius * sinf(static_cast<float>(i * std::numbers::pi) / 180.0f)  //
                                 };

        circle_points.push_back(pg::vec_cast<int>(p));
    }
    renderer.drawLines(std::bit_cast<SDL_Point*>(circle_points.data()), static_cast<int>(circle_points.size()));
}

void asteroids::RenderSystem::handle(const pg::game::FrameStamp&)
{
    auto& renderer = _game.getApp().getRenderer();
    renderer.clear();

    for (auto view = _game.getGlobalRegistry().view<pg::game::Drawable, pg::Transform2D>(); auto& entity : view)
    {
        auto&& [drawable, transform] = view.get<pg::game::Drawable, pg::Transform2D>(entity);
        drawable.prim->draw(renderer, transform, {});
    }

    auto renderConfig = _game.getCurrentScene().getSingleton<RenderConfig>();

    if (renderConfig.renderBroadPhaseCollisionShapes)
    {
        for (auto  boundsView = _game.getGlobalRegistry().view<pg::BoundingSphere, pg::Transform2D>();
             auto& entity : boundsView)
        {
            auto&& [bound, transform] = boundsView.get<pg::BoundingSphere, pg::Transform2D>(entity);
            renderSDL(renderer, bound, transform);
        }
    }
    renderer.present();
}
