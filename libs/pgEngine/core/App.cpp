#include <core/App.hpp>
#include "Lifetime.hpp"
#include <SDL_ttf.h>
#include <primitives/Primitives.hpp>

using namespace pg;

void pg::ttfInitDelegate()
{
    TTF_Init();
}

void pg::ttfQuitDelegate()
{
    TTF_Quit();
}

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
    ttfInit = std::make_unique<TTFInit>();
    window = std::make_unique<sdl::Window>(windowConfig.windowName.c_str(),
                                           windowConfig.offset[0],
                                           windowConfig.offset[1], //
                                           windowConfig.size[0],
                                           windowConfig.size[1], //
                                           SDL_WINDOW_BORDERLESS);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"); // TODO: move to config
    renderer = std::make_unique<sdl::Renderer>(window->get(), -1, SDL_RENDERER_ACCELERATED);
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

auto SDLApp::getWindowConfig() const -> const config::WindowConfig&
{
    return windowConfig;
}

auto pg::SDLApp::getFPSCounter() -> FPSCounter&
{
    return fpsCounter;
}

void SDLApp::loop(bool& done, const RenderFunction& renderFunc)
{
    getEventHandler().quit = [&done](const SDL_QuitEvent&) { done = true; };

    while (!done)
    {
        while (getEventHandler().poll()) {}
        // #TODO: configurable, if clear and which color
        {
            pg::ScopedColor sc{getRenderer(), Color{0, 0, 0, 255}};
            renderer->clear();
        }
        renderFunc(*this);
        renderer->present();
        fpsCounter.frame();
    }
}

void SDLApp::checkInitialized() const
{
    if (!init) { throw std::runtime_error("SDLApp not initialized"); }
}
