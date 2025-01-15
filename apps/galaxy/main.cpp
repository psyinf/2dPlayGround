#include <Galaxy.hpp>
#include <pgEngine/core/ErrorTrace.hpp>
#include <pgEngine/core/LoadSave.hpp>

#include <Config.hpp>
#include <serializer/ConfigSerializer.hpp>
#include <fmt/format.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    pg::game::GameConfig config;
    config.windowConfig.size = {800, 600};
    config.windowConfig.windowName = "Galaxy";

    config.vfsConfigs.push_back({.root = "../data", .alias = "data", .type = pg::game::VFSConfig::VFSType::PHYSICAL});
    config.vfsConfigs.push_back({.root = "../data/music/cylinders/Chris Zabriskie - Cylinders.zip",
                                 .alias = "music/cylinders",
                                 .type = pg::game::VFSConfig::VFSType::ZIP});

    {
        galaxy::GalacticCore gc(std::move(config));
        gc.setup();
        gc.run();
    }

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