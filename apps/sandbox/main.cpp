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

#include <Factories.h>
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
    pg::Transform testTransform{};

    // some callback that is executed directly when the key is pressed
    // this basically happens at the rate of key-repeat
    keyStateMap.registerDirectCallback(
        SDLK_x, {pg::KeyStateMap::CallbackTrigger::BOTH, {[](auto pressed) {
                     std::cout << "x "
                               << (pressed == pg::KeyStateMap::CallbackTrigger::PRESSED ? "pressed" : "released")
                               << "\n";
                 }}});
    // register callbacks to be executed when desired, e.g. once per frame, independent from the key-repeat
    keyStateMap.registerCallback(SDLK_a, [&testTransform](auto) { testTransform.pos[0] -= 10; });
    keyStateMap.registerCallback(SDLK_d, [&testTransform](auto) { testTransform.pos[0] += 10; });
    keyStateMap.registerCallback(SDLK_w, [&testTransform](auto) { testTransform.pos[1] -= 10; });
    keyStateMap.registerCallback(SDLK_s, [&testTransform](auto) { testTransform.pos[1] += 10; });
    pg::Line  l{pg::iVec2{0, 0}, pg::iVec2{1280, 720}};
    pg::Point p1{pg::iVec2{10, 10}};
    pg::Point p2{pg::iVec2{9, 9}};
    pg::Point p3{pg::iVec2{9, 9}};
    Circler   c({550, 550}, 100, 555);
    int       frame = 0;

    auto sprite = pg::SpriteFactory::makeSprite(renderer, "../data/playerShip1_blue.png");
    auto background = std::make_unique<pg::ScrollingSprite>(
        pg::SpriteFactory::makeSprite(renderer, "../data/grid_bg.png"), SDL_Rect{0, 0, 1280, 720});
    while (!done)
    {
        // handle all pending events
        while (sdlApp.getEventHandler().poll()) {}
        keyStateMap.evaluateCallbacks();

        renderer.setDrawColor(0x00, 0x00, 0x00, 0xff);
        renderer.clear();
        background->draw(renderer, testTransform);
        renderer.setDrawColor(0xff, 0xff, 0xff, 0xff);
        l.draw(renderer);

        renderer.setDrawColor(0xff, 0x00, 0x00, 0xff);
        p1.draw(renderer);
        p2.draw(renderer);
        p3.draw(renderer);

        sprite.draw(renderer, c.frame(++frame));
        ;
        renderer.present();
    }

    return 0;
}

catch (const std::exception& e)
{
    std::cerr << "Terminated: " << e.what() << std::endl;
    return -1;
}