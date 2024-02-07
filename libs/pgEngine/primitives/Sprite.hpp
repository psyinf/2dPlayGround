#pragma once
#include <pgEngine/math/Vec.hpp>
#include <pgEngine/primitives/Primitives.hpp>
#include <pgEngine/core/State.hpp>

namespace pg {

struct Sprite : public pg::Primitive
{
    Sprite(sdl::Texture&& tex);

    void draw(sdl::Renderer& r, const pg::Transform2D& t, const States& states) override;

    const pg::iVec2& getDimensions() const { return dimensions; }

protected:
    sdl::Texture& getTexture() { return texture; }
    
private:
    sdl::Texture texture;
    pg::iVec2    dimensions{};
};
} // namespace pg