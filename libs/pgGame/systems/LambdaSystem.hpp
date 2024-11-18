#pragma once
#include <pgGame/systems/SystemInterface.hpp>

namespace pg::game {

template <typename Func>
class LambdaSystem : public pg::game::SystemInterface
{
public:
    LambdaSystem(pg::game::Game& game, Func&& lambda)
      : pg::game::SystemInterface(game)
      , lambda(std::forward<Func>(lambda))
    {
    }

    void setup(std::string_view scene_id) override {}

    void handle(const pg::FrameStamp& frameStamp) override { lambda(); }

private:
    Func lambda;
};

template <typename Func>
auto makeLambdaSystem(pg::game::Game& game, Func&& f)
{
    return std::make_unique<LambdaSystem<Func>>(game, std::forward<Func>(f));
}
} // namespace pg::game