#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <SDLApp.h>
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

namespace pg {

class ScrollingSprite : public Sprite
{
public:
    ScrollingSprite(pg::Sprite&& sprite, SDL_Rect&& backgroundRect)
      : Sprite(std::move(sprite))
      , backgroundRect(std::move(backgroundRect))
    {
    }

    void draw(sdl::Renderer& r, const pg::Transform& trans) override
    {
        auto& texture_rect = getTextureRect();
        getTexture().query(nullptr, nullptr, &texture_rect.w, &texture_rect.h);
        auto t = trans;
        t.pos[0] %= texture_rect.w;
        if (t.pos[0] > 0) { t.pos[0] -= texture_rect.w; }
        t.pos[1] %= texture_rect.h;
        if (t.pos[1] > 0) { t.pos[1] -= texture_rect.h; }
        SDL_Rect src_1_rect{0, 0, texture_rect.w, texture_rect.h};
        SDL_Rect dst_1_rect{t.pos[0], t.pos[1], texture_rect.w, texture_rect.h};
       
        SDL_Rect src_2_rect{0, 0, -t.pos[0], texture_rect.h};
        SDL_Rect dst_2_rect{texture_rect.w + t.pos[0], + t.pos[1], -t.pos[0], texture_rect.h};

        SDL_Rect src_3_rect{0, 0, texture_rect.w, -t.pos[1]};
        SDL_Rect dst_3_rect{t.pos[0], texture_rect.h + t.pos[1], texture_rect.w, -t.pos[1]};

        SDL_Rect src_4_rect{0, 0, -t.pos[0], -t.pos[1]};
        SDL_Rect dst_4_rect{texture_rect.w + t.pos[0], texture_rect.h + t.pos[1], -t.pos[0], -t.pos[1]};

        r.copyEx(getTexture().get(), &src_1_rect, &dst_1_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
        r.copyEx(getTexture().get(), &src_2_rect, &dst_2_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
        r.copyEx(getTexture().get(), &src_3_rect, &dst_3_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
        r.copyEx(getTexture().get(), &src_4_rect, &dst_4_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
    }


    /*
           auto& texture_rect = getTextureRect();
       getTexture().query(nullptr, nullptr, &texture_rect.w, &texture_rect.h);
       auto t = trans;
       t.pos[0] %= texture_rect.w;
       if (t.pos[0] > 0) { t.pos[0] -= texture_rect.w; }
       t.pos[1] %= texture_rect.h;
       if (t.pos[1] > 0) { t.pos[1] -= texture_rect.h; }

       SDL_Rect src_1_rect{0, 0, texture_rect.w, texture_rect.h};
       SDL_Rect dst_1_rect{t.pos[0], t.pos[1], texture_rect.w, texture_rect.h};
       // horizontal
       SDL_Rect dst_2_rect{texture_rect.w + t.pos[0], +t.pos[1], -t.pos[0], texture_rect.h};
       SDL_Rect src_2_rect{0, 0, -t.pos[0], texture_rect.h + t.pos[1]};

       SDL_Rect src_3_rect{0, 0, texture_rect.w, -t.pos[1]};
       SDL_Rect dst_3_rect{t.pos[0], texture_rect.h + t.pos[1], texture_rect.w, -t.pos[1]};



       SDL_Rect src_4_rect{0, 0, -t.pos[0], -t.pos[1]};
       SDL_Rect dst_4_rect{texture_rect.w + t.pos[0], texture_rect.h - t.pos[1], -t.pos[0], t.pos[1]};
       r.copyEx(getTexture().get(), &src_1_rect, &dst_1_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
        r.copyEx(getTexture().get(), &src_2_rect, &dst_2_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
        r.copyEx(getTexture().get(), &src_3_rect, &dst_3_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
       // r.copyEx(getTexture().get(), &src_4_rect, &dst_4_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
    */
private:
    SDL_Rect backgroundRect;
};

} // namespace pg

int main(int argc, char** argv)
try
{
    pg::config::WindowConfig windowConfig{{0, 0}, {1024, 768}, "minimal demo"};
    pg::SDLApp               sdlApp{windowConfig};

    auto& renderer = sdlApp.getRenderer();
    auto  done = false;
    sdlApp.getEventHandler().quit = [&done](const SDL_QuitEvent&) {
        std::cout << "bye!";
        done = true;
    };
    pg::Transform testTransform{};
    sdlApp.getEventHandler().keyDown = [&testTransform](const SDL_KeyboardEvent& keyboardEvent) {
        // std::cout << "Pressed " << keyboardEvent.keysym << "\n";
        if (keyboardEvent.keysym.sym == SDLK_a) { testTransform.pos[0] -= 10; }
        if (keyboardEvent.keysym.sym == SDLK_d) { testTransform.pos[0] += 10; }
        if (keyboardEvent.keysym.sym == SDLK_w) { testTransform.pos[1] -= 10; }
        if (keyboardEvent.keysym.sym == SDLK_s) { testTransform.pos[1] += 10; }
        // std::cout << "Pressed " << SDL_GetKeyName(keyboardEvent.keysym.scancode) << std::endl;
    };
    pg::Line  l{pg::iVec2{0, 0}, pg::iVec2{1280, 720}};
    pg::Point p1{pg::iVec2{10, 10}};
    pg::Point p2{pg::iVec2{9, 9}};
    pg::Point p3{pg::iVec2{9, 9}};
    Circler   c({550, 550}, 100, 555);
    int       frame = 0;

    auto sprite = pg::SpriteFactory::makeSprite(renderer, "../data/playerShip1_blue.png");
    auto background = std::make_unique<pg::ScrollingSprite>(
        pg::SpriteFactory::makeSprite(renderer, "../data/raster.png"), SDL_Rect{0, 0, 1280, 720});
    while (!done)
    {
        // handle all pending events
        while (sdlApp.getEventHandler().poll()) {}
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