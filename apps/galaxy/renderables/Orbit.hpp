#pragma once
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/math/Transform.hpp>

#include <pgOrbit/OrbitalMechanics.hpp>

namespace galaxy {

class OrbitRenderable : public pg::Renderable
{
public:
    OrbitRenderable(const pgOrbit::OrbitalParameters<double>& op, int segments, pg::Color color)
      : _color(color)
    {
        _points.reserve(segments + 1);

        for (int i = 360; i-- > 0;)
        {
            // std constant for pi
            auto p = pgOrbit::OrbitalMechanics<double>::getEulerAnglesFromEccentricAnomaly(
                         op, i / 360.0 * 2 * std::numbers::pi)
                         .toCartesian();
            _points.push_back({static_cast<float>(p[0]), static_cast<float>(p[1])});
        }

        _points.push_back(_points[0]);
    }

    void draw(pg::Renderer& r, const pg::Transform2D& transform, const pg::States& rendererStates) override
    {
        // draw a circle for now
        pg::ScopedColor color(r.renderer, {255, 255, 255, 255});
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

        rendererStates.apply(r.renderer);
        r.renderer.drawLines(std::bit_cast<SDL_Point*>(local_points.data()), static_cast<int>(local_points.size()));
        rendererStates.restore(r.renderer);
    }

private:
    std::vector<pg::fVec2> _points;
    pg::Color              _color;
};

} // namespace galaxy