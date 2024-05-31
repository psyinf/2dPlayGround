#pragma once
#include <behaviortree_cpp/tree_node.h>
#include <unordered_map>
#include <unordered_set>

namespace behavior {

class PortInjector
{
public:
    PortInjector& input(const std::string& port, const std::string& value, bool forced = false)
    {
        inputPortInjections[port] = value;
        if (forced) { forcedInput.insert(port); }
        return *this;
    }

    PortInjector& output(const std::string& port, const std::string& value, bool forced = false)
    {
        outputPortInjections[port] = value;
        if (forced) { forcedOutput.insert(port); }
        return *this;
    }

    BT::NodeConfig injectPorts(const BT::NodeConfig& config)
    {
        BT::NodeConfig new_config = config;
        insertForcedPorts(new_config);
        for (const auto& port : new_config.input_ports)
        {
            if (inputPortInjections.find(port.first) != inputPortInjections.end())
            {
                new_config.input_ports[port.first] = inputPortInjections[port.first];
            }
        }
        for (const auto& port : new_config.output_ports)
        {
            if (outputPortInjections.find(port.first) != outputPortInjections.end())
            {
                new_config.output_ports[port.first] = outputPortInjections[port.first];
            }
        }

        return new_config;
    }

    void insertForcedPorts(BT::NodeConfig& config)
    {
        for (const auto& port : forcedInput)
        {
            if (!config.input_ports.contains(port)) { config.input_ports.insert({port, ""}); };
        }
        for (const auto& port : forcedOutput)
        {
            if (!config.output_ports.contains(port)) { config.output_ports.insert({port, ""}); };
        }
    }

private:
    std::unordered_map<std::string, std::string> inputPortInjections;

    std::unordered_map<std::string, std::string> outputPortInjections;
    std::unordered_set<std::string>              forcedInput;
    std::unordered_set<std::string>              forcedOutput;
};

} // namespace behavior