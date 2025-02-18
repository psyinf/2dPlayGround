#include "PythonScripter.hpp"
#include <pybind11/embed.h>

#include <pybind11/pybind11.h>

#include <fstream>
#include <ranges>

void pg::scripting::PythonScripter::run()

{
    namespace py = pybind11;
    py::scoped_interpreter guard{};
    // modules
    using ModulePtr = std::unique_ptr<py::module>;
    std::vector<py::module> modules;

    for (const auto& module : _modules)
    {
        modules.emplace_back(py::module::import(module.c_str()));
    }
    // script
    std::string script_str = _script | std::views::join | std::ranges::to<std::string>();
    py::exec(script_str);
}

pg::scripting::PythonScripter::PythonScripter(ScriptLines&& script, Modules&& /*= {}*/)
  : _script(std::move(script))
{
}

pg::scripting::ScriptLines pg::scripting::PythonScripter::scriptFromString(const std::string& script)
{
    ScriptLines       lines;
    std::stringstream ss(script);
    std::string       line;
    while (std::getline(ss, line, '\n'))
    {
        lines.push_back(line + "\n");
    }
    return lines;
}

pg::scripting::ScriptLines pg::scripting::PythonScripter::scriptFromFile(const std::filesystem::path& scriptPath)
{
    std::fstream file(scriptPath, std::ios_base::binary);
    if (!file.is_open()) { throw std::runtime_error("Failed to open file: " + scriptPath.string()); }

    ScriptLines lines;
    while (!file.eof())
    {
        std::string line;
        std::getline(file, line);
        lines.push_back(line + "\n");
    }
    return lines;
}
