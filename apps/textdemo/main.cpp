#include "SDL_ttf.h"
#include <Factories.h>
#include <SDLApp.h>
#include <SDLErrorTrace.h>
#include <SDLPrimitives.h>

void renderText()
{
    SDL_RenderPresent(nullptr);
    SDL_Delay(2000);
    /* SDL_FreeSurface(surfaceMessage);
     SDL_DestroyTexture(Message);
     TTF_CloseFont(font);*/
    TTF_Quit();
}

void textdemo_main()
{
    pg::SDLApp app{pg::config::WindowConfig{.screen{}, .offset{200, 200}, .size{1024, 1024}}};

    // make a text sprite
    auto text =
        pg::SpriteFactory::makeTextSprite(app.getRenderer(), "Hello World", "../data/fonts/Roboto-Regular.ttf", 24);
    // draw it
    float rot = 0.0;
    // TODO: test approach where we render the text as white and then use blending to color it
    auto render = [&](auto& app) {
        text.draw(app.getRenderer(),
                  {.pos{100, 100}, .rotation_deg{rot += 0.1}, .scale{static_cast<float>(std::sin(rot * 0.1)), 0.5}});
        text.draw(app.getRenderer(), {.pos{512, 512}});
    };

    auto done = false;
    app.loop(done, render);
}

int main(int argc, char** argv)
{
    textdemo_main();
    return 0;
}