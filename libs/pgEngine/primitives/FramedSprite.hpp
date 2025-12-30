#pragma once
#include <pgEngine/primitives/Sprite.hpp>
#include <pgEngine/math/VecOps.hpp>

namespace pg {

class FramedSprite : public pg::Sprite
{
public:
    using FrameCalculationFunction = std::function<void(int& current_frame)>;
    constexpr static auto DefaultFrameCalculationFunction = [](int& current_frame) { ++current_frame; };

    // TODO: supply a function that determines the current frame as parameter. Default could be a function that uses a
    // realtime clock and a duration set for the animation
    // TODO: this works for framed sprites that have all their frames in a single row. For more complex animations, a
    // more complex solution is needed
    FramedSprite(std::shared_ptr<sdl::Texture> tex,
                 uint16_t                      width,
                 uint16_t                      height,
                 FrameCalculationFunction      frameFunction)
      : Sprite(tex)
      , _frameFunction(frameFunction)
      , width(width)
      , height(height)
    {
    }

    void draw(pg::Renderer& r, const pg::Transform2D& t, const States& states) override
    {
        _frameFunction(_currentFrame);
        _currentFrame %= width * height;
        const auto& dims = getDimensions();
        const auto& fdimensions = pg::vec_cast<float>(dims) * pg::fVec2{1.0f / width, 1.0f / height};

        auto&     tex = getTexture();
        auto      calcPos = t.pos - (fdimensions * 0.5f * t.scale);
        SDL_FRect dest_rect = {calcPos[0], calcPos[1], (dims[0] * t.scale[0] / width), (dims[1] * t.scale[1] / height)};
        int       x = _currentFrame % width;
        int       y = _currentFrame / width;
        SDL_Rect  src_rect = {x * static_cast<int>(fdimensions[0]),
                              y * static_cast<int>(fdimensions[1]),
                              (dims[0] / width) + width,
                              (dims[1] / height)};
        states.apply(r.renderer);
        states.apply(r.renderer, tex);

        SDL_RenderCopyExF(r.renderer.get(), tex.get(), &src_rect, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);
        // r.copyExF(texture.get(), nullptr, &dest_rect, t.rotation_deg, nullptr, SDL_FLIP_NONE);

        states.restore(r.renderer);
        states.restore(r.renderer, tex);
    }

private:
    int                      _currentFrame{};
    FrameCalculationFunction _frameFunction;
    uint16_t                 width;
    uint16_t                 height;
};

} // namespace pg
