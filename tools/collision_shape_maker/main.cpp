#include <SDLApp.h>
#include <SDLBackgoundSprite.h>
#include <SDLErrorTrace.h>
#include <SDL_image.h>
#include <deque>
#include <fmt/format.h>
#include <ranges>

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
      , maxElement(points.size())
    {
    }

    void setMaxElement(size_t maxElement) { this->maxElement = maxElement; }

    size_t getMaxElement() const { return maxElement; }

    void draw(sdl::Renderer& r, const Transform& t) override
    {
        ScopedScale ss(r, t.scale);
        r.setDrawColor(255, 255, 255, 255);
        // draw the polygon
        for (const auto& p : points)
        {
            r.drawPoints(std::bit_cast<SDL_Point*>(points.data()), std::min(maxElement, points.size()));
        }
    }

    void draw(sdl::Renderer& r) { draw(r, {}); }

private:
    std::vector<iVec2>& points;
    size_t              maxElement{points.size()};
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

template <typename Vec2>
std::vector<Vec2> convexHull(const std::vector<Vec2>& points)
{
    auto lexicographical_compare = [](const Vec2& lhs, const Vec2& rhs) {
        return ((lhs[0] < rhs[0]) || ((lhs[0] == rhs[0]) && (lhs[1] < rhs[1])));
    };

    auto turn = [](Vec2 const& point1, Vec2 const& point2, Vec2 const& point3) {
        double z =
            (point2[0] - point1[0]) * (point3[1] - point1[1]) - (point2[1] - point1[1]) * (point3[0] - point1[0]);
        if (std::numeric_limits<float>::epsilon() >= std::fabs(z)) { return 0.0; }
        return z;
    };

    if (points.size() < 3) { throw std::invalid_argument("Insufficient number of points for convex hull"); }

    std::deque<Vec2> point_set = std::ranges::to<std::deque<Vec2>>(points);
    std::ranges::sort(point_set, lexicographical_compare);
    
    Vec2 left_most = point_set.front();
    Vec2 right_most = point_set.back();

    point_set.pop_back();
    point_set.pop_front();

    //split into upper/lower set;
    std::deque<Vec2> lower_set;
    std::deque<Vec2> upper_set;
    while (!point_set.empty())
    {
        if (turn(left_most, point_set.front(), right_most) < 0) { upper_set.push_back(point_set.front()); }
        else { lower_set.push_back(point_set.front()); }
        point_set.pop_front();
    }


    // construct lower hull
    auto lower_hull = std::deque<Vec2>{{left_most}}; 
    lower_set.push_back(right_most);

    while (!lower_set.empty())
    {
        lower_hull.push_back(lower_set.front());
        lower_set.pop_front();

        while ((lower_hull.size() >= 3) &&
               (turn(*(lower_hull.end() - 3), *(lower_hull.end() - 2), *(lower_hull.end() - 1)) < 0))
        {
            lower_hull.erase(lower_hull.end() - 2);
        }
    }
    lower_hull.pop_back();

    // construct upper hull
    auto upper_hull = std::deque<Vec2>{{right_most}};
    upper_set.push_front(left_most);

    while (!upper_set.empty())
    {
        upper_hull.push_back(upper_set.back());
        upper_set.pop_back();

        while ((upper_hull.size() >= 3) &&
               (turn(*(upper_hull.end() - 3), *(upper_hull.end() - 2), *(upper_hull.end() - 1)) < 0))
        {
            upper_hull.erase(upper_hull.end() - 2);
        }
    }
    upper_hull.pop_back();

    // merge upper and lower hull into hull
    std::vector<Vec2> hull(lower_hull.begin(), lower_hull.end());
    hull.insert(hull.end(), upper_hull.begin(), upper_hull.end());

    return hull;
}

void calculatePolygon(SpritePixelData& pixelData)
{
    auto frame = 0u;

    std::vector<pg::iVec2> points{};
    findPoints(points, pixelData);
    auto convPoints = convexHull(points);
    // TODO: minimize number of points by collapsing nearly co-linear lines
    pg::RefPoints rp{points};
    pg::RefPoints rpConvex{convPoints};
    rpConvex.setMaxElement(1);
    pg::SDLApp app{pg::config::WindowConfig{.screen{}, .offset{200, 200}, .size{pixelData.getDimensions() * 5}}};
    pg::Sprite sprite(sdl::Texture(app.getRenderer().get(), pixelData.getSurface().get()));

    auto fDims = pg::fVec2{float(pixelData.getDimensions()[0]), float(pixelData.getDimensions()[1])};

    auto render = [&](auto& app) {
        ++frame;
        if (frame % 10 == 0)
        {
            rpConvex.setMaxElement(rpConvex.getMaxElement() + 1);
            if (rpConvex.getMaxElement() > convPoints.size()) { rpConvex.setMaxElement(1); }
        }
        sprite.draw(app.getRenderer(), {.pos{fDims * 2.5f}, .scale{5, 5}});
        // rp.draw(app.getRenderer(), {.scale{5, 5}});
        rpConvex.draw(app.getRenderer(), {.scale{5, 5}});
    };

    auto done = false;
    app.loop(done, render);
}

int main(int argc, char* argv[])
try
{
    fmt::print("Usage: collision_shape_maker.exe <path_to_image>\n");
    fmt::print("Example: collision_shape_maker.exe ../data/meteorBrown_big1.png\n");
    //SpritePixelData sprite("../data/meteorBrown_big1.png");
    SpritePixelData sprite("../data/playerShip1_blue.png");

    //printASCII(sprite.getRawPixels(), sprite.getDimensions());
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