#include <primitives/Sprite.hpp>
#include <math/VecOps.hpp>

pg::Sprite::Sprite(std::shared_ptr<sdl::Texture> tex)
  : texture(tex)
{
    texture->query(nullptr, nullptr, &dimensions[0], &dimensions[1]);
}

void pg::Sprite::draw(pg::Renderer& r, const pg::Transform2D& t, const States& states)
{
    auto      calcPos = t.pos - (pg::vec_cast<float>(dimensions) * 0.5f * t.scale);
    SDL_FRect dest_rect = {calcPos[0], calcPos[1], (dimensions[0] * t.scale[0]), (dimensions[1] * t.scale[1])};
    states.apply(r.renderer);
    states.apply(r.renderer, *texture);
    // TODO: SDL_RenderCopyExF is not available in sdlpp
    SDL_RenderCopyExF(r.renderer.get(), texture->get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
    // r.copyExF(texture.get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);

    states.restore(r.renderer);
    states.restore(r.renderer, *texture);
}

void pg::Sprites::draw(pg::Renderer& r, const pg::Transform2D& t, const States& states)
{
    auto& tex = getTexture();
    auto  dims = pg::vec_cast<float>(getDimensions());
    states.apply(r.renderer);
    states.apply(r.renderer, tex);
    for (auto& transform : instanceTransforms)
    {
        auto box = pg::fBox{transform.pos, dims};
        auto b = box;
        b.pos -= box.midpoint();
        b.pos *= transform.scale * t.scale;
        b.dim *= transform.scale * t.scale;
        b.pos += b.midpoint();

        b.pos += t.pos;
        SDL_FRect dest_rect = {b.left(), b.top(), b.width(), b.height()};

        // TODO: SDL_RenderCopyExF is not available in sdlpp
        SDL_RenderCopyExF(r.renderer.get(), tex.get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
        // r.copyExF(texture.get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
    }
    states.restore(r.renderer);
    states.restore(r.renderer, tex);
}
