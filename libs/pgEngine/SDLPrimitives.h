#pragma once
#include "SDLVec.h"
#include <sdlpp.hpp>

namespace pg {

struct Transform
{
    fVec2 pos{};
    float rotation_deg{};
    fVec2 scale{1, 1};
};

class Primitive
{
public:
    virtual void draw(sdl::Renderer& r) = 0;

    virtual void draw(sdl::Renderer& r, const Transform& t) { draw(r); };
};

class Line : public Primitive
{
public:
    Line(iVec2&& start, iVec2&& end);

    void draw(sdl::Renderer& r) override;

protected:
    iVec2 start;
    iVec2 end;
};

class Point : Primitive
{
public:
    Point(iVec2&& pos)
      : pos(pos){};

    void draw(sdl::Renderer& r) override;

protected:
    iVec2 pos;
};

} // namespace pg