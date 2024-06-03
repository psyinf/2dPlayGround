#include <Galaxy.hpp>
#include <pgEngine/core/ErrorTrace.hpp>
#include <pgEngine/core/LoadSave.hpp>

#include <Config.hpp>
#include <serializer/ConfigSerializer.hpp>
#include <fmt/format.h>

int main(int argc, char** argv)
try
{
    galaxy::config::Galaxy config;
    pg::save("../data/galaxy_default_config.json", config);
    // config = pg::load<galaxy::config::Galaxy>("..data/galaxy_config.json");
    galaxy::GalacticCore gc(std::move(config));
    gc.setup();
    gc.run();

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