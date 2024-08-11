#include <primitives/Renderable.hpp>

pg::Line::Line(iVec2&& start, iVec2&& end)
  : start(start)
  , end(end)
{
}

void pg::Line::draw(pg::Renderer& r, const Transform2D&, const States&)
{
    r.renderer.drawLine(start[0], start[1], end[0], end[1]);
}

void pg::Point::draw(pg::Renderer& r, const Transform2D&, const States&)
{
    r.renderer.drawPoint(pos[0], pos[1]);
}
