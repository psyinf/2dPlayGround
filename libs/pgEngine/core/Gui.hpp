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
    ~Gui();

    void render(std::function<void()> rendering);

    void processEvent(const SDL_Event& event);

private:
    pg::SDLApp& app;
};
} // namespace pg