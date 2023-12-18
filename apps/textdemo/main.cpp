#include "SDL_ttf.h"
#include <SDLApp.h>
#include <SDLErrorTrace.h>
#include <SDLPrimitives.h>
#include <Factories.h>
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

   //make a text sprite
    auto      text = pg::SpriteFactory::makeTextSprite(
        app.getRenderer(), "Hello World", "../data/fonts/Roboto-Regular.ttf", 24, {255, 0, 0, 255});
    //draw it 
    float rot = 0.0;
    auto render = [&](auto& app) {
        // SDL_RenderCopy(app.getRenderer().get(), Message, nullptr, &Message_rect);
        text.draw(app.getRenderer(), {.pos{100, 100}, .rotation_deg{rot+=0.1}, .scale{0.5, 0.5}});    
    };

    auto done = false;
    app.loop(done, render);
}

int main(int argc, char** argv)
{
    textdemo_main();
    return 0;
}