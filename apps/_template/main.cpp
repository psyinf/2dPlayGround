
#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/core/ErrorTrace.hpp>
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/core/State.hpp>

void textdemo_main()
{
    pg::SDLApp app{pg::config::WindowConfig{.screen{}, .offset{200, 200}, .size{1024, 1024}}};

    auto font = pg::SDLFont("../data/fonts/Roboto-Regular.ttf", 24);
    // make a text sprite
    auto  text = pg::SpriteFactory::makeTextSprite(app.getRenderer(), font, "Hello, World!");
    auto  line = pg::Line{{0, 0}, {100, 100}};
    auto& renderer = app.getRenderer();
    renderer.setDrawColor(0xff, 0x00, 0xff, 0xff);

    auto render = [&](auto& app) {
        text.draw(app.getRenderer(), {.pos{100, 100}}, {});
        line.draw(app.getRenderer(), {.pos{100, 400}}, {});
    };

    auto done = false;
    app.loop(done, render);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    textdemo_main();
    return 0;
}