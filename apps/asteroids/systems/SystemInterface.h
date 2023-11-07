#pragma once
#include <entt/entt.hpp>
#include "core/FrameStamp.h"

namespace game {
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

protected:
    Game& game;
};
} // namespace game;
