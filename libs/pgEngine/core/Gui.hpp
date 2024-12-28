#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <functional>

namespace pg {
class SDLApp;

class Gui
{
public:
    Gui(pg::SDLApp& app);
    Gui(const Gui&) = delete;
    Gui& operator=(const Gui&) = delete;

    ~Gui();
    void begin();
    void end();
    void render(std::function<void()> rendering);

private:
    void bindEventProcessing();
    void prepareFonts();

    pg::SDLApp& app;
};
} // namespace pg