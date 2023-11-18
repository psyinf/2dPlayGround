#include <SDLVec.h>
#include <algorithm>
#include <deque>
#include <functional>
#include <limits>
#include <ranges>

namespace pg::compGeometry {

std::vector<pg::iVec2> findContour(pg::SpritePixelData& pixelData)
{
    std::vector<pg::iVec2> points;
    // TODO: this currently finds the points to the right at one pixel offset
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

    for (const auto y : std::views::iota(0, pixelData.getDimensions()[1]))
    {
        inside = false;
        for (const auto x : std::views::iota(0, pixelData.getDimensions()[0]))
        {
            // todo: incorporate "inside" to move x to the left if leaving
            if (next(pg::iVec2{x, y})) { inside ? points.push_back({x, y}) : points.push_back({x - 1, y}); }
            if (x == pixelData.getDimensions()[0]-1 && inside) { points.push_back({x, y}); }
        }
    }
    return points;
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

    // split into upper/lower set;
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
} // namespace pg::compGeometry