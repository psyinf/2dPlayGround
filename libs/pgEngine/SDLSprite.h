#pragma once
#include "SDLVec.h"
#include "SDLPrimitives.h"
#include "State.hpp"

namespace pg {

struct Sprite : public pg::Primitive
{
    Sprite(sdl::Texture&& tex);

    void draw(sdl::Renderer& r, const pg::Transform& t, const States& states); 

    const pg::iVec2& getDimensions() const { return dimensions; }

protected:
    sdl::Texture& getTexture() { return texture; }
    
private:
    sdl::Texture texture;
    pg::iVec2    dimensions{};
};
} // namespace pg