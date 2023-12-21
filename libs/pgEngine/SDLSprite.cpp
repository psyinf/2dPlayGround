#include "SDLSprite.h"

pg::Sprite::Sprite(sdl::Texture&& tex)
  : texture(std::move(tex))
{
    texture.query(nullptr, nullptr, &dimensions[0], &dimensions[1]);
}

void pg::Sprite::draw(sdl::Renderer& r, const pg::Transform& t, const RendererStates& states) {
    auto     new_x = int(t.pos[0] - (dimensions[0] / 2.0 * t.scale[0]));
    auto     new_y = int(t.pos[1] - (dimensions[1] / 2.0 * t.scale[1]));
    SDL_Rect dest_rect = {new_x, new_y, int(dimensions[0] * t.scale[0]), int(dimensions[1] * t.scale[1])};
    for (auto& state : states) {
        state->apply(r);
    }

    r.copyEx(texture.get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);

    for (auto& state : states)
    {
        state->restore(r);
    }
}
