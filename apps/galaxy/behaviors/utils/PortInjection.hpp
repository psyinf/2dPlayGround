#pragma once
#include <behaviortree_cpp/tree_node.h>
#include <unordered_map>

namespace behavior {

class PortInjector
{
public:
    PortInjector& input(const std::string& port, const std::string& value)
    {
        inputPortInjections[port] = value;
        return *this;
    }

    PortInjector& output(const std::string& port, const std::string& value)
    {
        outputPortInjections[port] = value;
        return *this;
    }

    BT::NodeConfig injectPorts(const BT::NodeConfig& config)
    {
        BT::NodeConfig new_config = config;
        for (const auto& port : config.input_ports)
        {
            if (inputPortInjections.find(port.first) != inputPortInjections.end())
            {
                new_config.input_ports[port.first] = inputPortInjections[port.first];
            }
        }
        for (const auto& port : config.output_ports)
        {
            if (outputPortInjections.find(port.first) != outputPortInjections.end())
            {
                new_config.output_ports[port.first] = outputPortInjections[port.first];
            }
        }
        return new_config;
    }

private:
    std::unordered_map<std::string, std::string> inputPortInjections;
    std::unordered_map<std::string, std::string> outputPortInjections;
};

} // namespace behavior