#include <SDLApp.h>
#include <SDLBackgoundSprite.h>
#include <SDLErrorTrace.h>
#include <SDL_image.h>
#include <fmt/format.h>
#include <span>

template <typename Type, size_t Extent>
void printASCII(const std::span<Type, Extent>& data, const pg::iVec2& dims)
{
    // get consecutive 4 bytes, and the index of the pixel
    for (const auto& [index, pixel] : std::views::enumerate(data | std::views::chunk(4)))
    {
        const auto [a, r, g, b] = std::tuple{pixel[0], pixel[1], pixel[2], pixel[3]};

        if (index % dims[0] == 0) { std::cout << "\n"; }
        // draw us a nice little ascii image based on the alpha value
        std::cout << (a != 0) ? "1" : "0";
    }
}

class SpritePixelData
{
public:
    SpritePixelData(const std::string_view pathToImage)
    {
        sdl::Surface spriteSurface(IMG_Load("../data/meteorBrown_big1.png"));
        surface = std::make_unique<sdl::Surface>(spriteSurface.get(), SDL_PIXELFORMAT_ARGB8888, 0);
        pixels =
            std::span{std::bit_cast<uint8_t*>(spriteSurface->pixels), size_t(spriteSurface->w * spriteSurface->h * 4)};
        dims = {spriteSurface->w, spriteSurface->h};
    }

public:
    const std::span<uint8_t>& getPixels() const { return pixels; }

    const pg::iVec2& getDimensions() const { return dims; }

private:
    std::unique_ptr<sdl::Surface> surface;
    std::span<uint8_t>            pixels;
    pg::iVec2                     dims{};
};

int main(int argc, char* argv[])
try
{
    fmt::print("Usage: collision_shape_maker.exe <path_to_image>\n");
    fmt::print("Example: collision_shape_maker.exe ../data/meteorBrown_big1.png\n");
    SpritePixelData sprite("../data/meteorBrown_big1.png");

    printASCII(sprite.getPixels(), sprite.getDimensions());
    // find contours based on alpha/key color
    // create a convex hull from the contours
    return 0;
}
catch (std::exception& e)
{
    fmt::print("Unhandled exception: {}\n", e.what());
    errorTrace::printErrorTrace();
    return -1;
}
catch (...)
{
    fmt::print("Unhandled exception");
    errorTrace::printErrorTrace();
    return -1;
}