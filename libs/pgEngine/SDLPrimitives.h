#pragma once
#include "SDLVec.h"
#include <sdlpp.hpp>

namespace pg {

class Primitive
{
public:
    virtual void draw(sdl::Renderer& r) = 0;
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