#pragma once
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/math/Transform.hpp>

namespace galaxy {

class OrbitRenderable : public pg::Renderable
{
public:
    OrbitRenderable(float radius, int segments, pg::Color color)
      : _color(color)
    {
        _points.reserve(segments + 1);
        for (int i = 0; i < segments; ++i)
        {
            float angle = 2.0f * std::numbers::pi_v<float> * i / segments;
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            _points.push_back({x, y});
        }
        _points.push_back(_points[0]);
    }

    void draw(sdl::Renderer& r, const pg::Transform2D& transform, const pg::States& rendererStates) override
    {
        // draw a circle for now
        pg::ScopedColor color(r, {255, 255, 255, 255});
        // pg::ScopedScale scale(r, t.scale);
        //  for now from lines
        std::vector<SDL_Point> local_points;
        // for now manual transform
        for (auto point : _points)
        {
            point *= transform.scale;
            point += transform.pos;

            local_points.push_back({static_cast<int>(point[0]), static_cast<int>(point[1])});
        }

        rendererStates.apply(r);
        r.drawLines(std::bit_cast<SDL_Point*>(local_points.data()), static_cast<int>(local_points.size()));
        rendererStates.restore(r);
    }

private:
    std::vector<pg::fVec2> _points;
    pg::Color              _color;
};

} // namespace galaxy