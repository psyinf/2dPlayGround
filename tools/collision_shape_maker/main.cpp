#include <SDLApp.h>
#include <SDLBackgoundSprite.h>
#include <SDLErrorTrace.h>
#include <SDL_image.h>
#include <fmt/format.h>
#include <span>

template <typename Type, size_t Extent>
void printASCII(const std::span<Type, Extent>& data, const pg::iVec2& dims)
{
    for (const auto& [index, pixel] : std::views::enumerate(data | std::views::chunk(4)))
    {
        auto [a, r, g, b] = std::tuple{pixel[0], pixel[1], pixel[2], pixel[3]};

        if (index % dims[0] == 0) { std::cout << "\n"; }
        std::cout << (a != 0) ? "1" : "0";
    }
}

int main(int argc, char** argv)
try
{
    fmt::print("Usage: collision_shape_maker.exe <path_to_image>\n");
    fmt::print("Example: collision_shape_maker.exe ../data/meteorBrown_big1.png\n");

    // 22,16,13
    sdl::Surface spriteSurface(IMG_Load("../data/meteorBrown_big1.png"));
    sdl::Surface formattedImage(spriteSurface.get(), SDL_PIXELFORMAT_ARGB8888, 0);
    std::span data{reinterpret_cast<uint8_t*>(spriteSurface->pixels), size_t(spriteSurface->w * spriteSurface->h * 4)};
    // for (auto [index, pixel] : std::views::enumerate(data | std::views::adjacent<4>))
    auto dims = pg::iVec2{spriteSurface->w, spriteSurface->h};
    printASCII(data, {});
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