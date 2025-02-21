#pragma once

#include <unordered_map>
#include <typeinfo>
#include <typeindex>

/* Type to Name mapping monostate*/
namespace pgf {
class NamedTypeRegistry
{
public:
    template <typename T>
    static void registerType(const std::string& name)
    {
        _registry[typeid(T)] = name;
    }

    template <typename T>
    const std::string& getTypeName() const
    {
        return _registry[typeid(T)];
    }

private:
    static inline std::unordered_map<std::type_index, std::string> _registry;
};
} // namespace pgf
