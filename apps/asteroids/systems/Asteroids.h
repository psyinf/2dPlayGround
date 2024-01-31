#pragma once
#include <systems/SystemInterface.hpp>
#include <array>
#include <deque>
#include <entt/entt.hpp>
#include <events/Collison.h>

namespace asteroids {
using entt::literals::operator""_hs;

class Game;
enum class Size
{
    Tiny,
    Small,
    Medium,
    Large
};

inline static constexpr std::optional<Size> getNextSmallest(Size size){
    switch (size)
    {
    case Size::Large:
        return Size::Medium;//
    case Size::Medium: 
        return Size::Small; //
    case Size::Small:
        return Size::Tiny; //
    case Size::Tiny:
        return std::nullopt;
    }
    return std::nullopt;
}

class Asteroids : public pg::game::SystemInterface
{
  
public:
    using SystemInterface::SystemInterface;

    using tag = entt::tag<"Asteroids"_hs>;

    void setup();

    void createAsteroid(const pg::fVec2& position, const pg::fVec2& velocity, Size size);

    void handle(const pg::game::FrameStamp& frameStamp);

    void handleEvent(const events::Collision& collision) { collisions.push_back(collision); }

private:

    std::deque<events::Collision> collisions;
};

} // namespace asteroids