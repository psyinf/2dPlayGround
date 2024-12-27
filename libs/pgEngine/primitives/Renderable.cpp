#include <primitives/Renderable.hpp>

pg::Line::Line(fVec2&& start, fVec2&& end)
  : start(start)
  , end(end)
{
}

void pg::Line::draw(pg::Renderer& r, const Transform2D& transform, const States& states)
{
    auto s = transformPoint(start, transform);
    auto e = transformPoint(end, transform);
    states.apply(r.renderer);
    SDL_RenderDrawLineF(r.renderer.get(), s[0], s[1], e[0], e[1]);
    states.restore(r.renderer);
}

void pg::Point::draw(Renderer& r, const Transform2D& transform, const States& rendererStates)
{
    auto p = transformPoint(_pos, transform);
    SDL_RenderDrawPointF(r.renderer.get(), p[0], p[1]);
}
