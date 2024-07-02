#include <Galaxy.hpp>
#include <pgEngine/core/ErrorTrace.hpp>
#include <pgEngine/core/LoadSave.hpp>

#include <Config.hpp>
#include <serializer/ConfigSerializer.hpp>
#include <fmt/format.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    {
        galaxy::GalacticCore gc;
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