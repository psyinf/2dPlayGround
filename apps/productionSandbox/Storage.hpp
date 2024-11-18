#pragma once

#include <unordered_map>
#include "Types.hpp"

struct Storage
{
    std::unordered_map<Name, Amount> resources;

    bool has(const Name& name) const { return resources.contains(name); }

    bool has_amount(const Name& name, Amount amount) const
    {
        if (!has(name)) { return false; }
        return resources.at(name) >= amount;
    }

    bool has_all(const std::vector<Requirement>& requirements, Fraction fraction) const
    {
        for (const auto& requirement : requirements)
        {
            if (!has_amount(requirement.name, requirement.amount * fraction)) { return false; }
        }
        return true;
    }

    Fraction get_max_amount(const std::vector<Requirement>& requirements) const
    {
        // get the amount of product we can build using requirements vs storage
        Fraction max_amount = std::numeric_limits<Fraction>::max();
        for (const auto& requirement : requirements)
        {
            if (!has(requirement.name)) { return {0}; }
            auto amount_in_storage = resources.at(requirement.name);
            auto fractions = amount_in_storage / requirement.amount;

            max_amount = std::min(resources.at(requirement.name) / requirement.amount, max_amount);
        }
        return max_amount;
    }

    void put(const Name& name, Amount amount) { resources[name] += amount; }

    void take(const Name& name, Amount amount) { resources[name] -= amount; }
};