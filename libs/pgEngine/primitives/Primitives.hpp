#pragma once

#include <pgEngine/math/Vec.hpp>
#include <pgEngine/core/States.hpp>
#include <pgEngine/math/Transform.hpp>
#include <pgEngine/math/Box.hpp>

#include <bit>
#include <memory>
#include <sdlpp.hpp>
#include <vector>

#include <SDL_rect.h>

namespace pg {

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

class Primitive
{
public:
    virtual void draw(sdl::Renderer& r, const Transform2D& t, const States& rendererStates) = 0;
};

class Placeholder : public Primitive
{
public:
    void draw(sdl::Renderer& r, const Transform2D& transform, const States& rendererStates) override
    {
        // do nothing
    }
};

class Group : public Primitive
{
public:
    void draw(sdl::Renderer& r, const Transform2D& transform, const States& rendererStates) override
    {
        for (auto& p : primitives)
        {
            p->draw(r, transform, rendererStates);
        }
    }

    void addPrimitive(std::unique_ptr<Primitive>&& p) { primitives.push_back(std::move(p)); }

private:
    std::vector<std::unique_ptr<Primitive>> primitives;
};

class Line : public Primitive
{
public:
    Line(iVec2&& start, iVec2&& end);

    void draw(sdl::Renderer& r, const Transform2D& transform, const States& rendererStates) override;

protected:
    iVec2 start;
    iVec2 end;
};

class Point : Primitive
{
public:
    Point(iVec2&& pos)
      : pos(pos){};

    void draw(sdl::Renderer& r, const Transform2D& transform, const States& rendererStates) override;

protected:
    iVec2 pos;
};

class LineStrip : public pg::Primitive
{
public:
    LineStrip(std::vector<iVec2>&& points)
      : points(points)
    {
    }

    void draw(sdl::Renderer& r, const Transform2D& transform, const States& rendererStates) override
    {
        r.setDrawColor(255, 255, 255, 255);
        // draw the polygon
        for (const auto& p : points)
        {
            r.drawLines(std::bit_cast<SDL_Point*>(points.data()), points.size());
        }
    }

private:
    std::vector<iVec2> points;
};

class BoxPrimitive : public pg::Primitive
{
public:
    BoxPrimitive(const pg::fBox& box, Color color = {255, 255, 255, 255})
      : box(box)
      , color(color)
    {
    }

    void draw(sdl::Renderer& r, const Transform2D& transform, const States& rendererStates) override
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
        pg::ScopedColor sc{r, color};
        // draw the polygon
        SDL_RenderDrawRectF(r.get(), &rect);
    }

private:
    const pg::fBox box;
    const Color    color;
};

class Points : public pg::Primitive
{
public:
    Points(std::vector<iVec2>&& points)
      : points(points)
    {
    }

    void draw(sdl::Renderer& r, const Transform2D& transform, const States& rendererStates) override
    {
        r.setDrawColor(255, 255, 255, 255);
        // draw the polygon
        for (const auto& p : points)
        {
            r.drawPoints(std::bit_cast<SDL_Point*>(points.data()), points.size());
        }
    }

private:
    std::vector<iVec2> points;
};

class RefPoints : public pg::Primitive
{
public:
    RefPoints(const std::vector<pg::iVec2>& points)
      : points(points)
      , maxElement(points.size())
    {
    }

    void setMaxElement(size_t maxElement) { this->maxElement = maxElement; }

    size_t getMaxElement() const { return maxElement; }

    void draw(sdl::Renderer& r, const Transform2D& transform, const States& rendererStates) override
    {
        ScopedScale ss(r, transform.scale);
        r.setDrawColor(255, 255, 255, 255);
        // draw the polygon
        for (const auto& p : points)
        {
            r.drawPoints(std::bit_cast<SDL_Point*>(points.data()), std::min(maxElement, points.size()));
        }
    }

private:
    const std::vector<iVec2>& points;
    size_t                    maxElement{points.size()};
};

class RefLines : public pg::Primitive
{
public:
    RefLines(const std::vector<pg::iVec2>& points)
      : points(points)
      , maxElement(points.size())
    {
    }

    void setMaxElement(size_t maxElement) { this->maxElement = maxElement; }

    size_t getMaxElement() const { return maxElement; }

    size_t size() const { return points.size(); }

    void draw(sdl::Renderer& r, const Transform2D& transform, const States& rendererStates) override
    {
        ScopedScale ss(r, transform.scale);
        r.setDrawColor(255, 255, 255, 255);
        // draw the polygon
        for (const auto& p : points)
        {
            r.drawLines(std::bit_cast<SDL_Point*>(points.data()), std::min(maxElement, points.size()));
        }
    }

private:
    const std::vector<iVec2>& points;
    size_t                    maxElement{points.size()};
};
} // namespace pg