#include <pgEngine/core/ErrorTrace.hpp>
#include <pgGame/core/Game.hpp>
#include <fmt/format.h>

#include <entities/StarSystem.hpp>
#include <systems/RenderSystem.hpp>

int main(int argc, char** argv)
try
{
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