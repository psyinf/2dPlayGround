#include "Game.h"
#include <SDLErrorTrace.h>
#include <fmt/format.h>

int main(int argc, char** argv)
try
{
    game::Game game;
    game.setup();
    game.loop();
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