#include <primitives/Sprite.hpp>

pg::Sprite::Sprite(std::shared_ptr<sdl::Texture> tex)
  : texture(tex)
{
    texture->query(nullptr, nullptr, &dimensions[0], &dimensions[1]);
}

void pg::Sprite::draw(sdl::Renderer& r, const pg::Transform2D& t, const States& states)
{
    auto      calcPos = t.pos - (vec_cast<float>(dimensions) * 0.5f * t.scale);
    SDL_FRect dest_rect = {calcPos[0], calcPos[1], (dimensions[0] * t.scale[0]), (dimensions[1] * t.scale[1])};
    states.apply(r);
    states.apply(r, *texture);
    // TODO: SDL_RenderCopyExF is not available in sdlpp
    SDL_RenderCopyExF(r.get(), texture->get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
    // r.copyExF(texture.get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);

    states.restore(r);
    states.restore(r, *texture);
}

void pg::Sprites::draw(sdl::Renderer& r, const pg::Transform2D& t, const States& states)
{
    for (auto& transform : instanceTransforms)
    {
        // add parent transform
        auto newTransform = t;
        newTransform.pos += transform.pos;
        newTransform.scale *= transform.scale;
        newTransform.rotation_deg += transform.rotation_deg;
        Sprite::draw(r, newTransform, states);
    }
}