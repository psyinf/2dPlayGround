#pragma once

#include <pgEngine/math/Vec.hpp>
#include <pgEngine/core/States.hpp>
#include <pgEngine/core/FrameStamp.hpp>
#include <pgEngine/math/Transform.hpp>
#include <pgEngine/math/Box.hpp>

#include <bit>
#include <memory>
#include <sdlpp.hpp>
#include <vector>

#include <SDL_rect.h>

namespace pg {
static constexpr auto white = SDL_Color{255, 255, 255, 255};

class Renderer;

class ScopedScale
{
public:
    ScopedScale(sdl::Renderer& r, const fVec2& setScale)
      : renderer(r)
    {
        renderer.getScale(&scale[0], &scale[1]);
        renderer.setScale(setScale[0], setScale[1]);
    }

    ~ScopedScale() { renderer.setScale(scale[0], scale[1]); }

private:
    sdl::Renderer& renderer;
    fVec2          scale;
};

class ScopedColor
{
public:
    ScopedColor(sdl::Renderer& r, const Color& setColor)
      : renderer(r)
    {
        renderer.getDrawColor(&color[0], &color[1], &color[2], &color[3]);
        renderer.setDrawColor(setColor[0], setColor[1], setColor[2], setColor[3]);
    }

    ~ScopedColor() { renderer.setDrawColor(color[0], color[1], color[2], color[3]); }

private:
    sdl::Renderer& renderer;
    Color          color;
};

class Renderer
{
public:
    sdl::Renderer&    renderer;
    const FrameStamp& frameStamp;

    void clear() { renderer.clear(); }

    void present() { renderer.present(); };

    void apply(States& states) { states.apply(renderer); }

    void restore(States& states) { states.restore(renderer); }
};

class Renderable
{
public:
    virtual ~Renderable() = default;
    virtual void draw(Renderer& r, const Transform2D& t, const States& rendererStates) = 0;

    static fVec2 transformPoint(const fVec2& point, const Transform2D& transform)
    {
        auto p = point;
        p *= transform.scale;
        p += transform.pos;
        return p;
    }
};

class Placeholder : public Renderable
{
public:
    void draw(Renderer&, const Transform2D&, const States&) override
    {
        // do nothing
    }
};

class Group : public Renderable
{
public:
    Group(std::vector<std::shared_ptr<Renderable>>&& primitives)
      : _primitives(std::move(primitives))
    {
    }

    Group() = default;

    void draw(Renderer& r, const Transform2D& transform, const States& rendererStates) override
    {
        for (auto& p : _primitives)
        {
            p->draw(r, transform, rendererStates);
        }
    }

    void addPrimitive(std::shared_ptr<Renderable>&& p) { _primitives.push_back(std::move(p)); }

private:
    std::vector<std::shared_ptr<Renderable>> _primitives;
};

// effectively a Group  with a switch
class Switch : public Group
{
    using Group::Group;

    Switch(std::vector<std::shared_ptr<Renderable>>&& primitives, bool enabled)
      : Group(std::move(primitives))
      , _enabled(enabled)
    {
    }

    Switch(bool enabled) { _enabled = enabled; }

public:
    void draw(Renderer& r, const Transform2D& transform, const States& rendererStates) override
    {
        if (_enabled) { Group::draw(r, transform, rendererStates); }
    }

    void setEnabled(bool enable) { _enabled = enable; }

    bool isEnabled() const { return _enabled; }

private:
    bool _enabled{true};
};

class Line : public Renderable

{
public:
    Line(fVec2&& start, fVec2&& end);

    void draw(Renderer& r, const Transform2D& transform, const States& rendererStates) override;

protected:
    fVec2 start;
    fVec2 end;
};

class Point : Renderable
{
public:
    Point(fVec2&& pos)
      : _pos(std::move(pos)) {};

    void draw(Renderer& r, const Transform2D& transform, const States& rendererStates) override;

protected:
    fVec2 _pos;
};

class LineStrip : public Renderable
{
public:
    LineStrip(std::vector<fVec2>&& points)
      : _points(points)
    {
    }

    void draw(Renderer& r, const Transform2D& transform, const States& states) override
    {
        // transform the points
        auto p = _points;
        for (auto& point : p)
        {
            // point -= (box.midpoint());
            point *= transform.scale;
            point += transform.pos;
            // point += (box.midpoint() * transform.scale);
        }
        states.apply(r.renderer);
        SDL_RenderDrawLinesF(r.renderer.get(), std::bit_cast<SDL_FPoint*>(p.data()), static_cast<int>(p.size()));
        states.restore(r.renderer);
    }

    void setPoints(std::vector<fVec2>&& points) { _points = std::move(points); }

private:
    std::vector<fVec2> _points;
};

class BoxPrimitive : public Renderable
{
public:
    BoxPrimitive(const fBox& box, Color color = {255, 255, 255, 255})
      : box(box)
      , color(color)
    {
    }

    void draw(Renderer& r, const Transform2D& transform, const States&) override
    {
        // transform the box
        auto b = box;
        b.pos -= (box.midpoint());
        b.pos *= transform.scale;
        b.dim *= transform.scale;
        b.pos += transform.pos;
        // TODO: this seems awkward
        b.pos += (box.midpoint() * transform.scale);
        auto rect = (SDL_FRect{b.left(), b.top(), b.width(), b.height()});
        // r.setDrawColor(255, 255, 255, 255);
        ScopedColor sc{r.renderer, color};
        // draw the polygon
        SDL_RenderDrawRectF(r.renderer.get(), &rect);
    }

private:
    const fBox  box;
    const Color color;
};

class Points : public Renderable
{
public:
    Points(std::vector<iVec2>&& points)
      : points(points)
    {
    }

    void draw(Renderer& r, const Transform2D&, const States&) override
    {
        r.renderer.setDrawColor(white.r, white.g, white.b, white.a);
        // draw the polygon

        r.renderer.drawPoints(std::bit_cast<SDL_Point*>(points.data()), static_cast<int>(points.size()));
    }

private:
    std::vector<iVec2> points;
};

class RefPoints : public Renderable
{
public:
    RefPoints(const std::vector<iVec2>& points)
      : points(points)
      , maxElement(points.size())
    {
    }

    void setMaxElement(size_t max_element) { this->maxElement = max_element; }

    size_t getMaxElement() const { return maxElement; }

    void draw(Renderer& r, const Transform2D& transform, const States&) override
    {
        ScopedScale ss(r.renderer, transform.scale);
        r.renderer.setDrawColor(white.r, white.g, white.b, white.a);

        r.renderer.drawPoints(std::bit_cast<SDL_Point*>(points.data()),
                              static_cast<int>(std::min(maxElement, points.size())));
    }

private:
    const std::vector<iVec2>& points;
    size_t                    maxElement{points.size()};
};

class RefLines : public Renderable
{
public:
    RefLines(const std::vector<iVec2>& points)
      : points(points)
      , maxElement(points.size())
    {
    }

    void setMaxElement(size_t max_element) { this->maxElement = max_element; }

    size_t getMaxElement() const { return maxElement; }

    size_t size() const { return points.size(); }

    void draw(Renderer& r, const Transform2D& transform, const States&) override
    {
        ScopedScale ss(r.renderer, transform.scale);
        r.renderer.setDrawColor(white.r, white.g, white.b, white.a);
        // draw the polygon
        r.renderer.drawLines(std::bit_cast<SDL_Point*>(points.data()),
                             static_cast<int>(std::min(maxElement, points.size())));
    }

private:
    const std::vector<iVec2>& points;
    size_t                    maxElement{points.size()};
};

class CircleSector : public LineStrip
{
public:
    CircleSector(fVec2&&  center,
                 float    radius,
                 float    startAngle,
                 float    endAngle,
                 uint32_t num_arc_points,
                 bool     draw_wedges = true)
      : LineStrip({})
    {
        auto       points = std::vector<fVec2>{};
        const auto numSegments = num_arc_points;
        const auto angleStep = (endAngle - startAngle) / numSegments;
        if (draw_wedges) { points.push_back(center); }
        for (auto i = 0u; i < numSegments; ++i)
        {
            const auto angle = startAngle + angleStep * i;
            const auto x = center[0] + radius * std::cos(angle);
            const auto y = center[1] + radius * std::sin(angle);
            points.emplace_back(fVec2{x, y});
        }
        points.emplace_back(fVec2{center[0] + radius * std::cos(endAngle), center[1] + radius * std::sin(endAngle)});
        if (draw_wedges) { points.push_back(center); }
        setPoints(std::move(points));
    }
};
} // namespace pg