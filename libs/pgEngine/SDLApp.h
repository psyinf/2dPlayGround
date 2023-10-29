#pragma once
#include "SDLAppConfig.h"
#include <sdlpp.hpp>

namespace pg {
class SDLApp
{
public:
    SDLApp(const config::WindowConfig& windowConfig);

    void initialize(const config::WindowConfig& windowConfig);

    auto getRenderer() -> sdl::Renderer&;

    auto getEventHandler() const -> sdl::EventHandler;

    void deinitialize();

    ~SDLApp();

private:
    config::WindowConfig           windowConfig;
    std::unique_ptr<sdl::Renderer> renderer;
    std::unique_ptr<sdl::Window>   window;
    std::unique_ptr<sdl::Init>     init;
    sdl::EventHandler              eventHandler;
};
}