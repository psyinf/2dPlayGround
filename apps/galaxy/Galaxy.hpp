#pragma once
#include <pgGame/core/Game.hpp>

namespace galaxy {

class GalacticCore

{
public:
    GalacticCore();

    void setup();

    void run();

private:
    std::unique_ptr<pg::game::Game> game;
};

} // namespace galaxy