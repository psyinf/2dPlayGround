#pragma once
#include <string>
#include <vector>
#include "FractionalAmount.hpp"
using FAmount = FractionalAmount<std::uint8_t, 4>;
// entities representing resources
using Amount = FAmount;
using Fraction = FAmount;
using Name = std::string;

// todo: traits that mark resources, intermediate products, and finished products etc

// Asset is a resource such as a raw material, a finished product, or energy
struct Asset
{
    // TODO: use a monostate to enforce that the name is unique
    // TODO: disallow empty names
    Asset(const Name& name)
      : name{name}
    {
    }

    virtual ~Asset() = default;
    const Name name;
};

struct Requirement
{
    Requirement(const Asset& asset, Amount amount = {1.0})
      : name{asset.name}
      , amount{amount}
    {
    }

    Name   name;
    Amount amount; // relative amount. e.g. if two requirements are needed, one with 1 and the other with 2 the ratio is
                   // 1:2 to produce a product
};

struct Product : public Asset
{
    template <typename... Args>
    Product(const Name& name, Args... args)
      : Asset{name}
      , _requirements{args...}
      , _fraction{{1.0f}}
    {
    }

    template <typename... Args>
    Product(const Name& name, float minimalFraction, Args... args)
      : Asset{name}
      , _requirements{args...}
      , _fraction{minimalFraction}
    {
    }

    const std::vector<Requirement> _requirements; // requirements to produce one item this product
    const Fraction _fraction{1.0f};   // minimal fraction of the product that can be produced.(parts of an item)
    const Fraction _complexity{1.0f}; // complexity of the product
};
