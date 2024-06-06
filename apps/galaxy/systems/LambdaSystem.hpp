#pragma once
#include <pgGame/systems/SystemInterface.hpp>

namespace galaxy {
template <typename Func>
class LambdaSystem : public pg::game::SystemInterface
{
public:
    LambdaSystem(pg::game::Game& game, Func&& lambda)
      : pg::game::SystemInterface(game)
      , lambda(std::forward<Func>(lambda))
    {
    }

    void setup() override {}

    void handle(const pg::game::FrameStamp& frameStamp) override { lambda(); }

private:
    Func lambda;
};
} // namespace galaxy