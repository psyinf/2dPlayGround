#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace pg::scripting {
using ScriptLines = std::vector<std::string>;
using Modules = std::vector<std::string>;

class PythonScripter
{
public:
    static ScriptLines scriptFromFile(const std::filesystem::path& scriptPath);
    static ScriptLines scriptFromString(const std::string& script);

    PythonScripter(ScriptLines&& script, Modules&& = {});

    void run();

    void addModule(const std::string& module) { _modules.push_back(module); }

private:
    ScriptLines _script;
    Modules     _modules;
};
} // namespace pg::scripting