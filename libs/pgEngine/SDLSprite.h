#pragma once
#include "SDLVec.h"
#include "SDLPrimitives.h"

namespace pg {

struct Sprite : public pg::Primitive
{
    Sprite(sdl::Texture&& tex);

    void draw(sdl::Renderer& r, const pg::Transform& t) override;

    void draw(sdl::Renderer& r) override;

    const pg::iVec2& getDimensions() const { return dimensions; }

protected:
    sdl::Texture& getTexture() { return texture; }
    
private:
    sdl::Texture texture;
    pg::iVec2    dimensions{};
};
} // namespace pg