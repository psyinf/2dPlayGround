#include <Galaxy.hpp>
#include <pgEngine/core/ErrorTrace.hpp>
#include <pgEngine/core/LoadSave.hpp>

#include <Config.hpp>
#include <serializer/ConfigSerializer.hpp>
#include <print>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    pg::game::GameConfig game_config;
    game_config.windowConfig.size = {800, 600};
    game_config.windowConfig.windowName = "Galaxy";

    game_config.vfsConfigs.push_back(
        {.root = "../data", .alias = "data", .type = pg::game::VFSConfig::VFSType::PHYSICAL});
    game_config.vfsConfigs.push_back(
        {.root = "../data", .alias = "::resources", .type = pg::game::VFSConfig::VFSType::PHYSICAL});
    game_config.vfsConfigs.push_back({.root = "../data/music/cylinders/Chris Zabriskie - Cylinders.zip",
                                      .alias = "music/soundtracks/cylinders",
                                      .type = pg::game::VFSConfig::VFSType::ZIP});
    {
        galaxy::GalacticCore gc(std::move(game_config));
        gc.setup();
        gc.run();
    }

    return 0;
}
catch (std::exception& e)
{
    std::print("Unhandled exception: {}\n", e.what());
    errorTrace::printErrorTrace();
    return -1;
}
catch (...)
{
    std::print("Unhandled exception");
    errorTrace::printErrorTrace();
    return -1;
}