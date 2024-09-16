
#include <string>
#include <vector>
#include <unordered_map>
#include <future>
// entities representing resources
using Amount = uint32_t;
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
    Requirement(const Asset& asset, Amount amount = 1)
      : name{asset.name}
      , amount{amount}
    {
    }

    Name   name;
    Amount amount;
};

struct Product : public Asset
{
    template <typename... Args>
    Product(const Name& name, Args... args)
      : Asset{name}
      , requirements{args...}
    {
    }

    template <typename... Args>
    void addRequirements(Args... args)
    {
        requirements.push_back(args...);
    }

    void addRequirement(Asset asset, Amount amount) { requirements.push_back({asset.name, amount}); }

    std::vector<Requirement> requirements;
};

struct Storage
{
    std::unordered_map<Name, Amount> resources;
};

struct Factory
{
    Factory(const Product& product)
      : product{product}
    {
    }

    // TODO: future?
    template <typename Produced>
    std::future<Produced> make(Storage& storage)
    {
        Produced p;
        // check and remove resources from storage
        // TODO: take time to produce into account

        return p;
    }
};

using Resource = Asset;
using Material = Product;

// example set of resources
const Resource Ores{"Ores"};
const Resource Rare_Ores{"Rare Ores"};
const Resource Energy{"Energy"};

Material metals{"Metals", Requirement{ores, 1}};

int main()
{
    // TODO: a storage
    //   a class of objects that can make a product from resources
    //   TODO: a production line
}