#include "SDLApp.h"
using namespace pg;

SDLApp::~SDLApp() {}

void SDLApp::deinitialize() {}

auto SDLApp::getEventHandler() const -> sdl::EventHandler
{
    return eventHandler;
}

auto SDLApp::getRenderer() -> sdl::Renderer&
{
    if (!renderer) { throw std::runtime_error("renderer not initialized"); }

    return *renderer;
}

void SDLApp::initialize(const config::WindowConfig& windowConfig)
{
    init = std::make_unique<sdl::Init>(SDL_INIT_EVERYTHING);
    window = std::make_unique<sdl::Window>(windowConfig.windowName.c_str(),
                                           windowConfig.offset[0],
                                           windowConfig.offset[1], //
                                           windowConfig.size[0],
                                           windowConfig.size[1], //
                                           SDL_WINDOW_BORDERLESS);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"); // TODO: move to config
    renderer = std::make_unique<sdl::Renderer>(window->get(), -1, 0);
}

SDLApp::SDLApp(const config::WindowConfig& windowConfig)
  : windowConfig(windowConfig)
{
    initialize(windowConfig);
}
