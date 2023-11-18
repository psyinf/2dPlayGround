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

namespace pg {



class RefPoints : public pg::Primitive
{
public:
    RefPoints(std::vector<pg::iVec2>& points)
      : points(points)
    {
    }

    void draw(sdl::Renderer& r, const Transform& t) override
    {
        ScopedScale ss(r, t.scale);
        r.setDrawColor(255, 255, 255, 255);
        // draw the polygon
        for (const auto& p : points)
        {
            r.drawPoints(std::bit_cast<SDL_Point*>(points.data()), points.size());
        }
    }

    void draw(sdl::Renderer& r) { draw(r, {}); }

private:
    std::vector<iVec2>& points;
};

} // namespace pg

void findPoints(std::vector<pg::iVec2>& points, SpritePixelData& pixelData)
{
    bool inside = false;
    auto next = [&inside, &pixelData](const pg::iVec2& pos) {
        auto val = std::get<0>(pixelData.getPixel(pos));
        if (val != 0 && !inside)
        {
            inside = true;
            return true;
        }
        if (val == 0 && inside)
        {
            inside = false;
            return true;
        }
        return false;
    };

    for (auto y : std::views::iota(0, pixelData.getDimensions()[1]))
    {
        inside = false;
        for (auto x : std::views::iota(0, pixelData.getDimensions()[0]))
        {
            if (next(pg::iVec2{x, y}))
            {
                std::cout << "1";
                points.push_back({x, y});
            }
            else { std::cout << " "; }
        }
        std::cout << "\n";
    }
}

void calculatePolygon(SpritePixelData& pixelData)
{
    std::vector<pg::iVec2> points{{0, 0}};
    pg::RefPoints          rp{points};
    findPoints(points, pixelData);

    pg::SDLApp app{pg::config::WindowConfig{.screen{}, .offset{200, 200}, .size{pixelData.getDimensions() * 5}}};

    pg::Sprite    sprite(sdl::Texture(app.getRenderer().get(), pixelData.getSurface().get()));
   
    auto          fDims = pg::fVec2{float(pixelData.getDimensions()[0]), float(pixelData.getDimensions()[1])};
    auto          render = [&](auto& app) {
        sprite.draw(app.getRenderer(), {.pos{fDims * 2.5f}, .scale{5, 5}});
        rp.draw(app.getRenderer(), {.scale{5, 5}});
    };

    auto done = false;
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
    calculatePolygon(sprite);
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