#include <primitives/Primitives.hpp>

pg::Line::Line(iVec2&& start, iVec2&& end)
  : start(start)
  , end(end)
{
}

void pg::Line::draw(sdl::Renderer& r, const Transform2D& transform, const States& rendererStates)
{
    r.drawLine(start[0], start[1], end[0], end[1]);
}

void pg::Point::draw(sdl::Renderer& r, const Transform2D& transform, const States& rendererStates)
{
    r.drawPoint(pos[0], pos[1]);
}
