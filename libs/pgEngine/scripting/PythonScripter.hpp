#pragma once
#include <filesystem>
#include <string>

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

private:
    ScriptLines _script;
    Modules     _modules;
};
} // namespace pg::scripting