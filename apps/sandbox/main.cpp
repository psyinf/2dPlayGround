#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <SDLApp.h>
#include <SDLBackgoundSprite.h>
#include <SDLKeyStateMap.h>
#include <SDLPrimitives.h>
#include <SDLVec.h>
#include <SDL_image.h>
#include <iostream>

#include <Factories.hpp>
#include <SDLSprite.h>
#include <vector>

class Circler
{
public:
    Circler(pg::iVec2&& p, int radius, int period_frames)
      : mid(std::move(p))
      , radius{radius}
      , period_frames(period_frames)
    {
    }

    pg::Transform frame(int frame)
    {
        pg::Transform t{};
        t.pos[0] = mid[0] + sin(frame / static_cast<float>(period_frames) * 3.1412) * radius;
        t.pos[1] = mid[1] + cos(frame / static_cast<float>(period_frames) * 3.1412) * radius;
        t.rotation_deg = 90 - (frame / static_cast<float>(period_frames) * 180);
        return t;
    }

protected:
    pg::iVec2 mid;
    int       radius;
    int       period_frames;
};

struct ColorState
{
    ColorState(const pg::Color& color)
      : setColor(color)
    {
        std::puts("S");
    }

    ~ColorState() { std::puts("~S"); }

    ColorState(const ColorState& rhs) = delete;
    ColorState(ColorState&& rhs) = delete;

    void operator=(const ColorState& rhs) = delete;
    void operator=(ColorState&& rhs) = delete;

    void set(sdl::Renderer& renderer, std::any& state)
    {
        pg::Color color;
        renderer.getDrawColor(&color[0], &color[1], &color[2], &color[3]);
        state = color;
        renderer.setDrawColor(setColor[0], setColor[1], setColor[2], setColor[3]);
    }

    void reset(sdl::Renderer& renderer, std::any& state)
    {
        pg::Color storedColor = std::any_cast<pg::Color>(state);
        renderer.setDrawColor(storedColor[0], storedColor[1], storedColor[2], storedColor[3]);
    }

    pg::Color setColor;
};

int main(int argc, char** argv)
try
{
    pg::config::WindowConfig windowConfig{0, {0, 0}, {1024, 768}, "minimal demo"};
    pg::SDLApp               sdlApp{windowConfig};
    pg::KeyStateMap          keyStateMap(sdlApp.getEventHandler());
    auto&                    renderer = sdlApp.getRenderer();
    auto                     done = false;
    sdlApp.getEventHandler().quit = [&done](const SDL_QuitEvent&) {
        std::cout << "bye!";
        done = true;
    };
    pg::Transform bgTransform{};

    // some callback that is executed directly when the key is pressed
    // this basically happens at the rate of key-repeat
    keyStateMap.registerDirectCallback(
        SDLK_x, {pg::KeyStateMap::CallbackTrigger::BOTH, {[](auto pressed) {
                     std::cout << "x "
                               << (pressed == pg::KeyStateMap::CallbackTrigger::PRESSED ? "pressed" : "released")
                               << "\n";
                 }}});
    // register callbacks to be executed when desired, e.g. once per frame, independent from the key-repeat
    keyStateMap.registerCallback(SDLK_a, [&bgTransform](auto) { bgTransform.pos[0] -= 10; });
    keyStateMap.registerCallback(SDLK_d, [&bgTransform](auto) { bgTransform.pos[0] += 10; });
    keyStateMap.registerCallback(SDLK_w, [&bgTransform](auto) { bgTransform.pos[1] -= 10; });
    keyStateMap.registerCallback(SDLK_s, [&bgTransform](auto) { bgTransform.pos[1] += 10; });
    pg::Line l{pg::iVec2{0, 0}, pg::iVec2{1280, 720}};
    pg::Line l2{pg::iVec2{5, 5}, pg::iVec2{1285, 725}};

    pg::Point p1{pg::iVec2{10, 10}};
    pg::Point p2{pg::iVec2{9, 9}};
    pg::Point p3{pg::iVec2{9, 9}};
    Circler   c({550, 550}, 100, 555);
    int       frame = 0;

    auto sprite = pg::SpriteFactory::makeSprite(renderer, "../data/playerShip1_blue.png");
    auto background = std::make_unique<pg::ScrollingSprite>(
        pg::SpriteFactory::makeSprite(renderer, "../data/grid_bg.png"), pg::iVec2{1280, 720});

    while (!done)
    {
        // handle all pending events
        while (sdlApp.getEventHandler().poll()) {}
        keyStateMap.evaluateCallbacks();

        renderer.setDrawColor(0x00, 0x00, 0x00, 0xff);
        renderer.clear();
        background->draw(renderer, bgTransform, {});

        renderer.setDrawColor(0xff, 0x00, 0xff, 0xff);
        l.draw(renderer, {}, {});
        l2.draw(renderer, {}, {});

        renderer.setDrawColor(0xff, 0x00, 0x00, 0xff);
        p1.draw(renderer, {}, {});
        p2.draw(renderer, {}, {});
        p3.draw(renderer, {}, {});

        sprite.draw(renderer, c.frame(++frame), {});

        renderer.present();
    }
    return 0;
}

catch (const std::exception& e)
{
    std::cerr << "Terminated: " << e.what() << std::endl;
    return -1;
}