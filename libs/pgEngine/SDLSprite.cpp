#include "SDLSprite.h"

pg::Sprite::Sprite(sdl::Texture&& tex)
  : texture(std::move(tex))
{
    texture.query(nullptr, nullptr, &dimensions[0], &dimensions[1]);
}

void pg::Sprite::draw(sdl::Renderer& r, const pg::Transform& t, const States& states)
{
    auto calcPos = t.pos - (vec_cast<float>(dimensions) * 0.5f * t.scale);
    SDL_FRect dest_rect = {calcPos[0], calcPos[1], (dimensions[0] * t.scale[0]), (dimensions[1] * t.scale[1])};
    states.apply(r);
    states.apply(r, texture);
    //TODO: SDL_RenderCopyExF is not available in sdlpp
    SDL_RenderCopyExF(r.get(), texture.get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
    //r.copyExF(texture.get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);

    states.restore(r);
    states.restore(r, texture);
}
