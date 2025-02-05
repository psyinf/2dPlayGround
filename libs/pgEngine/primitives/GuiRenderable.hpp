#pragma once

#include <pgEngine/core/Gui.hpp>

namespace pg::game {

class GuiRenderable
{
public:
    GuiRenderable() = default;

    virtual void draw(pg::Gui& gui) = 0;

private:
};

class GuiBegin : public GuiRenderable
{
public:
    virtual void draw(pg::Gui& gui) override { gui.begin(); }
};

class GuiEnd : public GuiRenderable
{
public:
    virtual void draw(pg::Gui& gui) override { gui.end(); }
};
} // namespace pg::game
