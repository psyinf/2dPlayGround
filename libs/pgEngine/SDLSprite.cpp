#include "SDLSprite.h"



 pg::Sprite::Sprite(sdl::Texture&& tex)
  : texture(std::move(tex))
{
    texture.query(nullptr, nullptr, &texture_rect.w, &texture_rect.h);
}

void pg::Sprite::draw(sdl::Renderer& r)
{
    SDL_Rect texture_rect{};
    texture.query(nullptr, nullptr, &texture_rect.w, &texture_rect.h);
    r.copy(texture.get(), nullptr, &texture_rect);
}

void pg::Sprite::draw(sdl::Renderer& r, const pg::Transform& t)
{
    texture.query(nullptr, nullptr, &texture_rect.w, &texture_rect.h);

    auto     new_x = int(t.pos[0] + texture_rect.w / 2) - int(t.scale[0] * texture_rect.w / 2);
    auto     new_y = int(t.pos[1] + texture_rect.h / 2) - int(t.scale[1] * texture_rect.h / 2);
    SDL_Rect dest_rect = {new_x, new_y, int(texture_rect.w * t.scale[0]), int(texture_rect.h * t.scale[1])};
    r.copyEx(texture.get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
}
