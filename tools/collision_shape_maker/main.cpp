#include <ComputationalGeometry.hpp>
#include <SDLApp.h>
#include <SDLErrorTrace.h>
#include <SDLPrimitives.h>
#include <SDLSprite.h>
#include <SDL_image.h>
#include <SpritePixelData.hpp>
#include <deque>
#include <fmt/format.h>
#include <ranges>

void printASCII(const std::span<uint8_t>& data, const pg::iVec2& dims)
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

void printASCII(const std::span<pg::SpritePixelData::Pixel>& data, const pg::iVec2& dims)
{
    // get consecutive 4 bytes, and the index of the pixel
    for (const auto& [index, pixel] : std::views::enumerate(data))
    {
        const auto& [a, r, g, b] = pixel;

        if (index % dims[0] == 0) { std::cout << "\n"; }
        // draw us a nice little ascii image based on the alpha value
        std::cout << (a != 0) ? "1" : "0";
    }
}

void debugDisplay(pg::SpritePixelData&          pixelData,
                  const std::vector<pg::iVec2>& contour,
                  const std::vector<pg::iVec2>& convexHull)
{
    auto          frame = 0u;
    pg::RefPoints rp{contour};
    pg::SDLApp    app{pg::config::WindowConfig{.screen{}, .offset{200, 200}, .size{pixelData.getDimensions() * 5}}};
    pg::Sprite    sprite(sdl::Texture(app.getRenderer().get(), pixelData.getSurface().get()));

    pg::RefLines rpConvex{convexHull};
    rpConvex.setMaxElement(1);
    auto fDims = pg::fVec2{float(pixelData.getDimensions()[0]), float(pixelData.getDimensions()[1])};

    auto render = [&](auto& app) {
        ++frame;
        if (frame % 100 == 0)
        {
            rpConvex.setMaxElement(rpConvex.getMaxElement() + 1);
            if (rpConvex.getMaxElement() > rpConvex.size()) { rpConvex.setMaxElement(1); }
        }
        sprite.draw(app.getRenderer(), {.pos{fDims * 2.5f}, .scale{5, 5}});
        // rp.draw(app.getRenderer(), {.scale{5, 5}});
        rpConvex.draw(app.getRenderer(), {.scale{5, 5}});
    };

    auto done = false;
    app.loop(done, render);
}

auto calculateConvexHull(pg::SpritePixelData& pixelData)
{
    std::vector<pg::iVec2> points = pg::compGeometry::findContour(pixelData);
    auto                   convPoints = pg::compGeometry::convexHull(points);
    convPoints = pg::compGeometry::mergeColinear(convPoints, 0.02);
    return std::make_tuple(points, convPoints);
}

int main(int argc, char* argv[])
try
{
    fmt::print("Usage: collision_shape_maker.exe <path_to_image>\n");
    fmt::print("Example: collision_shape_maker.exe ../data/meteorBrown_big1.png\n");
    // pg::SpritePixelData sprite("../data/meteorBrown_big1.png");
    // pg::SpritePixelData sprite("../data/laserBlue01.png");
    pg::SpritePixelData sprite("../data/playerShip1_blue.png");

    printASCII(sprite.getPixels(), sprite.getDimensions());
    auto [contour, convexHull] = calculateConvexHull(sprite);
    debugDisplay(sprite, contour, convexHull);
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