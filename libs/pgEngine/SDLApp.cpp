#include "SDLApp.h"
#include <ranges>
using namespace pg;

SDLApp::~SDLApp() {}

void SDLApp::deinitialize() {}

auto SDLApp::getEventHandler() -> sdl::EventHandler&
{
    return eventHandler;
}

auto SDLApp::getRenderer() -> sdl::Renderer&
{
    checkInitialized();

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

void SDLApp::getNumDisplays() const
{
    checkInitialized();
    SDL_GetNumVideoDisplays();
}

SDL_Rect pg::SDLApp::getDisplayBounds(const uint8_t screenNumber) const
{
    checkInitialized();
    SDL_Rect screen;
    SDL_GetDisplayBounds(screenNumber, &screen);
    return screen;
}

void SDLApp::loop(bool& done, const RenderFunction& renderFunc)
{
   
    getEventHandler().quit = [&done](const SDL_QuitEvent&) {
        
        done = true;
    };

    while (!done)
    {
        while (getEventHandler().poll()) {}
        //#TODO: configurable, if clear and which color
        renderer->setDrawColor(0x00, 0x00, 0x00, 0xff);
        renderer->clear();
        renderFunc(*this);
        renderer->present();
    }
}

void SDLApp::checkInitialized() const
{
    if (!init) { throw std::runtime_error("SDLApp not initialized"); }
}
