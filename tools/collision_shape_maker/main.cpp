#include <SDLApp.h>
#include <SDLBackgoundSprite.h>
#include <SDLErrorTrace.h>
#include <SDL_image.h>
#include <fmt/format.h>

/**
 * @brief Simple sprite surface wrapper
 * @todo Use mdspan
 */
class SpritePixelData
{
public:
    using ColorType = uint8_t;
    using Pixel = std::tuple<ColorType, ColorType, ColorType, ColorType>;
    static constexpr auto numPixelComponents{3};

    SpritePixelData(const std::string_view pathToImage)
    {
        sdl::Surface spriteSurface(IMG_Load("../data/meteorBrown_big1.png"));
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

private:
    std::unique_ptr<sdl::Surface> surface;
    std::span<uint8_t>            rawPixels;
    std::span<Pixel>              pixels;
    pg::iVec2                     dims{};
};

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

void printASCII(const std::span<SpritePixelData::Pixel>& data, const pg::iVec2& dims)
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

void drawPolygon(SpritePixelData& pixelData)
{
    pg::SDLApp app{pg::config::WindowConfig{.screen{}, .offset{200,200}, .size{pixelData.getDimensions()}}};
    auto       done = false;
    app.getEventHandler().quit = [&done](const SDL_QuitEvent&) {
        std::cout << "bye!";
        done = true;
    };
    pg::Sprite sprite(sdl::Texture(app.getRenderer().get(), pixelData.getSurface().get()));

    auto render = [&sprite](auto& app) { sprite.draw(app.getRenderer()); };

    app.loop(done, render);
}

int main(int argc, char* argv[])
try
{
    fmt::print("Usage: collision_shape_maker.exe <path_to_image>\n");
    fmt::print("Example: collision_shape_maker.exe ../data/meteorBrown_big1.png\n");
    SpritePixelData sprite("../data/meteorBrown_big1.png");

    printASCII(sprite.getRawPixels(), sprite.getDimensions());
    printASCII(sprite.getPixels(), sprite.getDimensions());
    drawPolygon(sprite);
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