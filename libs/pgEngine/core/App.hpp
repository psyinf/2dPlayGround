#pragma once
#include <pgEngine/core/AppConfig.hpp>
#include <pgEngine/core/FPSCounter.hpp>
#include <pgFoundation/Lifetime.hpp>

#include <sdlpp.hpp>

namespace pg {
// delegates for TTF_Init and TTF_Quit
void ttfQuitDelegate();
void ttfInitDelegate();

class SDLApp
{
    using TTFInit = Lifetime<ttfInitDelegate, ttfQuitDelegate>;

public:
    using RenderFunction = std::function<void(SDLApp&)>;
    using EventCallback = std::function<bool(SDL_Event)>;
    SDLApp(const config::WindowConfig& windowConfig);
    ~SDLApp();

    void initialize();

    auto getRenderer() -> sdl::Renderer&;

    auto getEventHandler() -> sdl::EventHandler&;

    auto getWindow() -> sdl::Window&;

    void deinitialize();

    void getNumDisplays() const;

    SDL_Rect getDisplayBounds(const uint8_t screen) const;

    auto getWindowConfig() const -> const config::WindowConfig&;

    auto getFPSCounter() -> FPSCounter&;
    //     [[deprecated("Use Game class")]] void loop(bool&                 done,
    //                                                const RenderFunction& renderFunction,
    //                                                const EventCallback&  eventCallback = {});

protected:
    void checkInitialized() const;

private:
    config::WindowConfig           windowConfig;
    std::unique_ptr<sdl::Renderer> renderer;
    std::unique_ptr<sdl::Window>   window;
    std::unique_ptr<sdl::Init>     init;
    std::unique_ptr<TTFInit>       ttfInit;
    sdl::EventHandler              eventHandler;
    FPSCounter                     fpsCounter;
};
} // namespace pg
