
#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/core/ErrorTrace.hpp>
#include <pgEngine/primitives/Primitives.hpp>
#include <pgEngine/core/State.hpp>

void textdemo_main()
{
    pg::SDLApp app{pg::config::WindowConfig{.screen{}, .offset{200, 200}, .size{1024, 1024}}};

    auto font = pg::SDLFont("../data/fonts/Roboto-Regular.ttf", 24);
    // make a text sprite
    auto text = pg::SpriteFactory::makeTextSprite(app.getRenderer(), font, "Hello World");
    auto text2 = pg::SpriteFactory::makeTextSprite(app.getRenderer(), font, "Hello World");
    // draw it
    float rot = 0.0;

    auto rendererStates = pg::States{};
    auto rendererStates2 = pg::States{};
    auto rendererStates3 = pg::States{};
    
    rendererStates.push(pg::TextureColorState{pg::Color{255, 0, 0, 255}});
    rendererStates2.push(pg::TextureColorState{pg::Color{255, 255, 0, 255}});
    rendererStates2.push(pg::TextureAlphaState{128});
    rendererStates3.push(pg::TextureColorState{pg::Color{255, 0, 255, 255}});
   

    
    auto render = [&](auto& app) {
       
        text.draw(app.getRenderer(),
                  {.pos{100, 100}, .rotation_deg{rot += 0.1}, .scale{static_cast<float>(std::sin(rot * 0.1)), 0.5}},
                  rendererStates);

        text.draw(app.getRenderer(), {.pos{512, 512}}, rendererStates2);
        
        text.draw(app.getRenderer(),
                   {.pos{200, 200}, .rotation_deg{rot += 0.1}, .scale{static_cast<float>(std::sin(rot * 0.1)), 0.5}},
                   rendererStates3);
        text.draw(app.getRenderer(), {.pos{512, 200}}, {});
        };

    auto done = false;
    app.loop(done, render);
}

int main(int argc, char** argv)
{
    textdemo_main();
    return 0;
}