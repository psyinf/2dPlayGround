#pragma once
#include <pgEngine/math/Vec.hpp>
#include <pgEngine/primitives/Primitives.hpp>
#include <pgEngine/core/State.hpp>

namespace pg {

struct Sprite : public pg::Primitive
{
    Sprite(std::shared_ptr<sdl::Texture> tex);

    void draw(sdl::Renderer& r, const pg::Transform2D& t, const States& states) override;

    const pg::iVec2& getDimensions() const { return dimensions; }

protected:
    sdl::Texture& getTexture() { return *texture; }

private:
    std::shared_ptr<sdl::Texture> texture;
    pg::iVec2                     dimensions{};
};

struct Sprites : public pg::Sprite
{
    using InstanceTransforms = std::vector<pg::Transform2D>;
    using Sprite::Sprite;

    Sprites(std::shared_ptr<sdl::Texture> tex)
      : Sprite(tex)
    {
    }

    void draw(sdl::Renderer& r, const pg::Transform2D& t, const States& states) override;

    InstanceTransforms& getTransforms() { return instanceTransforms; }

private:
    InstanceTransforms instanceTransforms;
};
} // namespace pg