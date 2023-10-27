#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <SDLApp.h>
#include <iostream>

#include <vector>

struct Star
{
    float x;
    float y;
    float z;
};

static void mainloop(pg::SDLApp& sdlApp, std::vector<Star>& stars)
{
 


}

int main(int argc, char** argv)
try
{
    pg::config::WindowConfig windowConfig{{65, 126}, {1280, 720}, "minimal demo"};
    pg::SDLApp               sdlApp{windowConfig};


      auto& renderer = sdlApp.getRenderer();
    auto  done = false;
    sdlApp.getEventHandler().quit = [&done](const SDL_QuitEvent&) {
        std::cout << "bye!";
        done = true;
    };

    while (!done)
    {
        while (sdlApp.getEventHandler().poll()) {}
        renderer->setDrawColor(0x00, 0x00, 0x00, 0xff);
        renderer->clear();
        renderer->setDrawColor(0xff, 0xff, 0xff, 0xff);
        renderer->drawLine(0, 0, 1280, 720);
        renderer->rectangleColor(100, 100, 200, 200, 0xff00ff00);
        renderer->present();
    }

    return 0;
}
catch (const std::exception& e)
{
    std::cerr << "Terminated: " << e.what() << std::endl;
    return -1;
}