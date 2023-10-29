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
    
    auto& renderer = sdlApp.getRenderer();
    auto done = false;
    sdlApp.getEventHandler().quit = [&done](const SDL_QuitEvent&) { std::cout << "bye!"; done = true; };

    while (!done)
    {
        while (sdlApp.getEventHandler().poll()) {}
        renderer.setDrawColor(0x00, 0x00, 0x00, 0xff);
        renderer.clear();
        for (auto& star : stars)
        {
            if (star.z < 0xff) star.z += 1;
            star.x = (star.x - 1280 / 2) * (1 + 0.0001 * star.z) + 1280 / 2;
            star.y = (star.y - 720 / 2) * (1 + 0.0001 * star.z) + 720 / 2;
            if (star.x < 0 || star.x > 1280 || star.y < 0 || star.y > 720)
                star = Star{1.0f * (rand() % 1280), 1.0f * (rand() % 720), 0};
            renderer.setDrawColor(star.z, star.z, star.z, 0xff);
            renderer.drawPoint(star.x, star.y);
        }
        renderer.present();
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
    pg::config::WindowConfig windowConfig{{65, 126}, {1280, 720}, "minimal demo"};
    pg::SDLApp       sdlApp{windowConfig};
        
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