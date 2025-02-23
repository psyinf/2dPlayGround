#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/scripting/PythonScripter.hpp>

namespace asteroids {

using entt::literals::operator""_hs;

class ScriptSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    virtual ~ScriptSystem() = default;

    void setup(std::string_view /*scene_id*/) override
    {
        _scripter = std::make_unique<pg::scripting::PythonScripter>(
            pg::scripting::PythonScripter::scriptFromFile("../data/scripts/test.py"));
        _scripter->addModule("mymodule");
    }

    void handle(const pg::FrameStamp& /*frameStamp*/) override { _scripter->run(); }

private:
    std::unique_ptr<pg::scripting::PythonScripter> _scripter;
};
} // namespace asteroids
