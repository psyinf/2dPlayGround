#pragma once
#include <pgGame/core/FrameStamp.hpp>

namespace pg::game {
class Game;

class SystemInterface
{
public:
    SystemInterface(Game& game)
      : game(game)
    {
    }

    virtual ~SystemInterface() = default;
    virtual void setup() = 0;
    virtual void handle(const FrameStamp& frameStamp) = 0;

    virtual void enterScene([[maybe_unused]] std::string_view newScene) {}

    virtual void exitScene([[maybe_unused]] std::string_view newScene) {}

protected:
    Game& game;
};
} // namespace pg::game
