#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <array>
#include <iostream>
#include <sdlpp.hpp>
#include <vector>
#include <memory>
struct Star
{
    float x;
    float y;
    float z;
};

namespace config {

template <typename ELEMENT_TYPE>
using Vec2 = std::array<ELEMENT_TYPE, 2>;

using Vec2Window = Vec2<int32_t>;

struct WindowConfig
{
    Vec2Window  offset;
    Vec2Window  size;
    std::string windowName;
};

// TODO: renderconfig -> renderhint

} // namespace config

class SDLApplication
{
public:
    SDLApplication(const config::WindowConfig& windowConfig)
      : windowConfig(windowConfig)
    {
        initialize(windowConfig);
    }

    void initialize(const config::WindowConfig& windowConfig)
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

   auto& getRenderer()
    {
        if (!renderer) { throw std::runtime_error("renderer not initialized"); }

        return renderer;
    }

    auto getEventHandler() const { return eventHandler; }

    void deinitialize() {}

    ~SDLApplication(){}

private:
    config::WindowConfig           windowConfig;
    std::unique_ptr<sdl::Renderer> renderer;
    std::unique_ptr<sdl::Window>   window;
    std::unique_ptr<sdl::Init>     init;
    sdl::EventHandler              eventHandler;

};

static void mainloop(SDLApplication& sdlApp, std::vector<Star>& stars)
{
    
    auto& renderer = sdlApp.getRenderer();
    auto done = false;
    sdlApp.getEventHandler().quit = [&done](const SDL_QuitEvent&) { std::cout << "bye!"; done = true; };

    while (!done)
    {
        while (sdlApp.getEventHandler().poll()) {}
        renderer->setDrawColor(0x00, 0x00, 0x00, 0xff);
        renderer->clear();
        for (auto& star : stars)
        {
            if (star.z < 0xff) star.z += 1;
            star.x = (star.x - 1280 / 2) * (1 + 0.0001 * star.z) + 1280 / 2;
            star.y = (star.y - 720 / 2) * (1 + 0.0001 * star.z) + 720 / 2;
            if (star.x < 0 || star.x > 1280 || star.y < 0 || star.y > 720)
                star = Star{1.0f * (rand() % 1280), 1.0f * (rand() % 720), 0};
            renderer->setDrawColor(star.z, star.z, star.z, 0xff);
            renderer->drawPoint(star.x, star.y);
        }
        renderer->present();
    }

    /*
    if (!game_is_still_running)
    {
        deinitialize_the_game();
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop(); // this should "kill" the app.
#else
        exit(0);
#endif
    }

    check_for_new_input();
    think_about_stuff();
    draw_the_next_frame();
    */
}

int main(int argc, char** argv)
try
{
    config::WindowConfig windowConfig{{65, 126}, {1280, 720}, "minimal demo"};
    SDLApplication       sdlApp{windowConfig};
        
    std::vector<Star> stars;
    for (int i = 0; i < 1000; ++i)
        stars.push_back(Star{1.0f * (rand() % 1280), 1.0f * (rand() % 720), 0});
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
#else
    mainloop(sdlApp, stars);
#endif
    return 0;
}
catch (const std::exception& e)
{
    std::cerr << "Terminated: " << e.what() << std::endl;
    return -1;
}