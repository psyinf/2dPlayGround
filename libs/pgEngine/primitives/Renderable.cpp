#include <primitives/Renderable.hpp>

pg::Line::Line(fVec2&& start, fVec2&& end)
  : start(start)
  , end(end)
{
}

void pg::Line::draw(pg::Renderer& r, const Transform2D& transform, const States& states)
{
    // transform the point
    auto s = start;
    auto e = end;
    // point -= (box.midpoint());
    s *= transform.scale;
    e *= transform.scale;
    s += transform.pos;
    e += transform.pos;

    // point += (box.midpoint() * transform.scale);
    states.apply(r.renderer);
    SDL_RenderDrawLineF(r.renderer.get(), s[0], s[1], e[0], e[1]);
    states.restore(r.renderer);
}

void pg::Point::draw(pg::Renderer& r, const Transform2D&, const States&)
{
    r.renderer.drawPoint(pos[0], pos[1]);
}
