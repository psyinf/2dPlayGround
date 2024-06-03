#pragma once
#include <pgEngine/primitives/Sprite.hpp>
#include <pgEngine/math/VecOps.hpp>

namespace pg {

class FramedSprite : public pg::Sprite
{
public:
    FramedSprite(std::shared_ptr<sdl::Texture> tex, uint16_t width, uint16_t height)
      : Sprite(tex)
      , width(width)
      , height(height)
    {
    }

    void draw(sdl::Renderer& r, const pg::Transform2D& t, const States& states) override
    {
        // TODO: get from extern
        uint32_t    currentFrame = 0;
        const auto& dimensions = getDimensions();
        const auto& fdimensions = pg::vec_cast<float>(dimensions) * pg::fVec2{1.0f / width, 1.0f / height};
        auto&       texture = getTexture();
        auto        calcPos = t.pos - (fdimensions * 0.5f * t.scale);
        SDL_FRect   dest_rect = {
            calcPos[0], calcPos[1], (dimensions[0] * t.scale[0] / width), (dimensions[1] * t.scale[1] / height)};
        int      x = currentFrame % width;
        int      y = currentFrame / width;
        SDL_Rect src_rect = {x * (int)fdimensions[0],
                             y * (int)fdimensions[1],
                             (dimensions[0] / width) + width,
                             (dimensions[1] / height)};
        states.apply(r);
        states.apply(r, texture);

        SDL_RenderCopyExF(r.get(), texture.get(), &src_rect, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
        // r.copyExF(texture.get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);

        states.restore(r);
        states.restore(r, texture);
    }

private:
    uint16_t width;
    uint16_t height;
};

} // namespace pg