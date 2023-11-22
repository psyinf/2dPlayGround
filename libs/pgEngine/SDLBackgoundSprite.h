#pragma once 
#include "SDLSprite.h"

namespace pg {

class ScrollingSprite : public Sprite
{
public:
    ScrollingSprite(pg::Sprite&& sprite, iVec2&& backgroundSize)
      : Sprite(std::move(sprite))
      , backgroundSize(std::move(backgroundSize))
    {
    }

    void draw(sdl::Renderer& r, const pg::Transform& trans) override
    {
        auto& dims = getDimensions();
       
        auto pos = iVec2{static_cast<int>(trans.pos[0]), static_cast<int>(trans.pos[1])};
        pos[0] %= dims[0];
        if (pos[0] > 0) { pos[0] -= dims[0]; }
        pos[1] %= dims[1];
        if (pos[1] > 0) { pos[1] -= dims[1]; }
        SDL_Rect src_1_rect{0, 0, dims[0], dims[1]};
        SDL_Rect dst_1_rect{pos[0], pos[1], dims[0], dims[1]};

        SDL_Rect src_2_rect{0, 0, -pos[0], dims[1]};
        SDL_Rect dst_2_rect{dims[0] + pos[0], +pos[1], -pos[0], dims[1]};

        SDL_Rect src_3_rect{0, 0, dims[0], -pos[1]};
        SDL_Rect dst_3_rect{pos[0], dims[1] + pos[1], dims[0], -pos[1]};

        SDL_Rect src_4_rect{0, 0, -pos[0], -pos[1]};
        SDL_Rect dst_4_rect{dims[0] + pos[0], dims[1] + pos[1], -pos[0], -pos[1]};

        r.copyEx(getTexture().get(), &src_1_rect, &dst_1_rect, trans.rotation_deg, nullptr, SDL_FLIP_NONE);
        r.copyEx(getTexture().get(), &src_2_rect, &dst_2_rect, trans.rotation_deg, nullptr, SDL_FLIP_NONE);
        r.copyEx(getTexture().get(), &src_3_rect, &dst_3_rect, trans.rotation_deg, nullptr, SDL_FLIP_NONE);
        r.copyEx(getTexture().get(), &src_4_rect, &dst_4_rect, trans.rotation_deg, nullptr, SDL_FLIP_NONE);
    }

private:
    iVec2 backgroundSize;
};

} // namespace pg