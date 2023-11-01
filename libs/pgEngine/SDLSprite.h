#pragma once
#include "SDLVec.h"
#include "SDLPrimitives.h"

namespace pg {

struct Sprite : public pg::Primitive
{
    Sprite(sdl::Texture&& tex);

    void draw(sdl::Renderer& r, const pg::Transform& t) override;

    void draw(sdl::Renderer& r) override;

protected:
    sdl::Texture& getTexture() { return texture; }
    SDL_Rect& getTextureRect() { return texture_rect; }
private:
    sdl::Texture texture;
    SDL_Rect     texture_rect{};
};
} // namespace pg