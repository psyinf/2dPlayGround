#pragma once
#include <math/Vec.hpp>
#include <sdlpp.hpp>
#include <SDL_image.h>
#include <bit>
#include <span>
#include <string>
#include <tuple>

namespace pg {

/**
 * @brief Simple sprite surface wrapper* @todo Use mdspan
 */
class SpritePixelData
{
public:
    using ColorType = uint8_t;
    using Pixel = std::tuple<ColorType, ColorType, ColorType, ColorType>;
    static constexpr auto numPixelComponents{3};

    SpritePixelData(const std::string_view pathToImage)
    {
        sdl::Surface spriteSurface(IMG_Load(pathToImage.data()));
        surface = std::make_unique<sdl::Surface>(spriteSurface.get(), SDL_PIXELFORMAT_ARGB8888, 0);
        rawPixels = std::span{std::bit_cast<uint8_t*>(spriteSurface->pixels),
                              size_t(spriteSurface->w * spriteSurface->h * numPixelComponents)};
        pixels = std::span{std::bit_cast<Pixel*>(spriteSurface->pixels), size_t(spriteSurface->w * spriteSurface->h)};

        dims = {spriteSurface->w, spriteSurface->h};
    }

public:
    const std::span<uint8_t>& getRawPixels() const { return rawPixels; }

    const std::span<Pixel>& getPixels() const { return pixels; }

    const pg::iVec2& getDimensions() const { return dims; }

    sdl::Surface& getSurface() { return *surface; }

    const Pixel getPixel(const pg::iVec2& pos) const
    {
        auto coord = pos[0] + (pos[1] * dims[0]);
        return pixels[coord];
    }

private:
    std::unique_ptr<sdl::Surface> surface;
    std::span<uint8_t>            rawPixels;
    std::span<Pixel>              pixels;
    pg::iVec2                     dims{};
};

} // namespace pg