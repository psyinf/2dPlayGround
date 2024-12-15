#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <pgEngine/core/App.hpp>
#include <pgEngine/primitives/BackgoundSprite.hpp>
#include <pgGame/core/KeyStateMap.hpp>
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/math/Vec.hpp>
#include <SDL_image.h>
#include <iostream>

#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/primitives/Sprite.hpp>
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

    pg::Transform2D frame(int frame)
    {
        pg::Transform2D t{};
        t.pos[0] = mid[0] + sin(frame / static_cast<float>(period_frames) * 3.1412f) * radius;
        t.pos[1] = mid[1] + cos(frame / static_cast<float>(period_frames) * 3.1412f) * radius;
        t.rotation_deg = 90 - (frame / static_cast<float>(period_frames) * 180.0f);
        return t;
    }

protected:
    pg::iVec2 mid;
    int       radius;
    int       period_frames;
};

static void setRendererDrawColor(sdl::Renderer& renderer, SDL_Color color)
{
    renderer.setDrawColor(color.r, color.g, color.b, color.a);
}

class DropAnimation : public pg::Sprite
{
public:
    DropAnimation(std::shared_ptr<sdl::Texture> tex, float scale = 1.0f, size_t duration = 60)
      : Sprite(tex)
      , _maxFrame(duration)
      , _scale(scale)
    {
    }

    void draw(pg::Renderer& r, const pg::Transform2D& t, const pg::States& states) override
    {
        auto trans = t;
        // IDEA scale non-linearly
        trans.scale *= (_frame / static_cast<float>(_maxFrame) * _scale);
        auto alpha_state =
            pg::TextureAlphaState{static_cast<uint8_t>(255u - (_frame / static_cast<float>(_maxFrame)) * 255u)};
        states.apply(r.renderer);
        states.apply(r.renderer, getTexture());
        alpha_state.apply(r.renderer, getTexture());
        Sprite::draw(r, trans, {});
        states.restore(r.renderer, getTexture());
        states.restore(r.renderer);
        _frame++;
        if (_frame > _maxFrame) { _frame = 0; }
    }

private:
    int    _frame = 0;
    float  _scale = 1.0f;
    size_t _maxFrame = 75;
};
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    pg::config::WindowConfig         windowConfig{0, {0, 0}, {1024, 768}, "minimal demo"};
    pg::SDLApp                       sdlApp{windowConfig};
    std::shared_ptr<pg::KeyStateMap> keyStateMap = std::make_shared<pg::KeyStateMap>();
    pg::InputEventDispatcher         inputEventDispatcher(sdlApp.getEventHandler(), {{"main", keyStateMap}});
    inputEventDispatcher.setHandlerActive("main", true);

    auto& sdl_renderer = sdlApp.getRenderer();
    auto  done = false;
    sdlApp.getEventHandler().quit = [&done](const SDL_QuitEvent&) {
        std::cout << "bye!";
        done = true;
    };
    pg::Transform2D bgTransform{};
    pg::Transform2D mouseClickTransform{};
    // some callback that is executed directly when the key is pressed
    // this basically happens at the rate of key-repeat
    keyStateMap->registerDirectCallback(
        SDLK_x, {pg::KeyStateMap::CallbackTrigger::BOTH, {[](auto pressed) {
                     std::cout << "x "
                               << (pressed == pg::KeyStateMap::CallbackTrigger::PRESSED ? "pressed" : "released")
                               << "\n";
                 }}});
    // register callbacks to be executed when desired, e.g. once per frame, independent from the key-repeat
    keyStateMap->registerKeyCallback(SDLK_a, [&bgTransform](auto) { bgTransform.pos[0] -= 10; });
    keyStateMap->registerKeyCallback(SDLK_d, [&bgTransform](auto) { bgTransform.pos[0] += 10; });
    keyStateMap->registerKeyCallback(SDLK_w, [&bgTransform](auto) { bgTransform.pos[1] -= 10; });
    keyStateMap->registerKeyCallback(SDLK_s, [&bgTransform](auto) { bgTransform.pos[1] += 10; });
    keyStateMap->registerMousePressedCallback(
        [&mouseClickTransform](auto pos, [[maybe_unused]] auto state, [[maybe_unused]] bool updown) {
            mouseClickTransform.pos = pg::vec_cast<float>(pos);
        });
    keyStateMap->registerMouseDraggedCallback([&mouseClickTransform](auto pos, [[maybe_unused]] auto state) {
        mouseClickTransform.pos = pg::vec_cast<float>(pos);
    });
    pg::Line l{pg::iVec2{0, 0}, pg::iVec2{1280, 720}};
    pg::Line l2{pg::iVec2{5, 5}, pg::iVec2{1285, 725}};

    pg::Point p1{pg::iVec2{10, 10}};
    pg::Point p2{pg::iVec2{9, 9}};
    pg::Point p3{pg::iVec2{9, 9}};
    Circler   c({550, 550}, 100, 555);
    int       frame = 0;

    auto sprite = pg::SpriteFactory::makeSprite(sdl_renderer, "../data/playerShip1_blue.png");
    auto background =
        std::make_unique<pg::ScrollingSprite>(pg::SpriteFactory::makeSprite(sdl_renderer, "../data/grid_bg.png"));
    auto animation = pg::SpriteFactory::makeFramedSprite(sdl_renderer, 8, 4, "../data/effects/explosion_1_8x4.png");

    auto blip = DropAnimation(
        std::make_shared<sdl::Texture>(pg::SpriteFactory::makeTexture(sdl_renderer, "../data/gui/ring.tga")));

    pg::FrameStamp frameStamp;
    pg::Renderer   renderer{sdl_renderer, frameStamp};
    while (!done)
    {
        // handle all pending events
        while (sdlApp.getEventHandler().poll()) {}
        keyStateMap->evaluateCallbacks();

        setRendererDrawColor(renderer.renderer, {0, 0, 0, 255});
        renderer.clear();
        background->draw(renderer, bgTransform, {});

        setRendererDrawColor(renderer.renderer, {255, 0, 255, 255});
        l.draw(renderer, {}, {});
        l2.draw(renderer, {}, {});

        setRendererDrawColor(renderer.renderer, {255, 0, 0, 255});

        p1.draw(renderer, {}, {});
        p2.draw(renderer, {}, {});
        p3.draw(renderer, {}, {});

        sprite.draw(renderer, c.frame(++frame), {});
        blip.draw(renderer, {pg::Transform2D{.pos{100, 100}}}, {});
        animation.draw(renderer, mouseClickTransform, {});
        renderer.present();
    }
    return 0;
}

catch (const std::exception& e)
{
    std::cerr << "Terminated: " << e.what() << std::endl;
    return -1;
}