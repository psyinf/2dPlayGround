#pragma once

#include <pgEngine/primitives/Renderable.hpp>

namespace pg::game {

class GuiDrawable : public Renderable
{
public:
    GuiDrawable(pg::Gui& gui)
      : gui(gui)
    {
    }

    pg::Gui& getGui() { return gui; }

private:
    pg::Gui& gui;
};

class GuiBeginDrawable : public GuiDrawable
{
public:
    using GuiDrawable::GuiDrawable;

    virtual void draw(sdl::Renderer& r, const Transform2D& t, const States& rendererStates) override
    {
        getGui().begin();
        bool show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);
    }
};

class GuiEndDrawable : public GuiDrawable
{
public:
    using GuiDrawable::GuiDrawable;

    virtual void draw(sdl::Renderer& r, const Transform2D& t, const States& rendererStates) override { getGui().end(); }
};
} // namespace pg::game
