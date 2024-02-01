
#include <factories/Factories.hpp>
#include <SDLApp.h>
#include <SDLErrorTrace.h>
#include <SDLPrimitives.h>
#include <State.hpp>

void textdemo_main()
{
    pg::SDLApp app{pg::config::WindowConfig{.screen{}, .offset{200, 200}, .size{1024, 1024}}};

    auto font = pg::SDLFont("../data/fonts/Roboto-Regular.ttf", 24);
    // make a text sprite
    auto text = pg::SpriteFactory::makeTextSprite(app.getRenderer(), font, "Hello, World!");
    auto line = pg::Line{{0, 0}, {100, 100}};
    auto& renderer = app.getRenderer();
    renderer.setDrawColor(0xff, 0x00, 0xff, 0xff);
    
    auto render = [&](auto& app) {
        text.draw(app.getRenderer(), {.pos{100, 100}}, {});   
        line.draw(app.getRenderer(), {.pos{100, 400}}, {});
    };

    auto done = false;
    app.loop(done, render);
}

int main(int argc, char** argv)
{
    textdemo_main();
    return 0;
}