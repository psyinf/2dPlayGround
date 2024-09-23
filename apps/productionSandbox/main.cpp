#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <future>
#include <expected>
// entities representing resources
using Amount = float;
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
    Amount amount; // relative amount. e.g. if two requirements are needed, one with 1 and the other with 2 the ratio is
                   // 1:2 to produce a product
};

struct Product : public Asset
{
    template <typename... Args>
    Product(const Name& name, Args... args)
      : Asset{name}
      , _requirements{args...}
      , _fraction{1.0f}
    {
    }

    template <typename... Args>
    Product(const Name& name, float minimalFraction, Args... args)
      : Asset{name}
      , _requirements{args...}
      , _fraction{minimalFraction}
    {
    }

    const std::vector<Requirement> _requirements;   // requirements to produce this product
    const float                    _fraction{1.0f}; // minimal fraction of the product that can be produced.
};

struct Storage
{
    std::unordered_map<Name, Amount> resources;
};

struct Factory
{
    using Task = std::function<void()>;
    using TaskList = std::vector<Task>;
    Factory() = default;

    std::optional<TaskList> plan(Storage& input_storage, Storage& output_storage, const Product& product)
    {
        // list of operations to reduce the amount of resources in storage
        TaskList operations;
        // check and remove resources from storage
        for (const auto& requirement : product._requirements)
        {
            if (input_storage.resources[requirement.name] < requirement.amount) { return std::nullopt; }
            operations.emplace_back(
                [&input_storage, &requirement]() { input_storage.resources[requirement.name] -= requirement.amount; });
        }
        operations.emplace_back(
            [&output_storage, &product]() mutable { output_storage.resources[product.name] += product._fraction; });
        // all requirements are met, remove resources
        return operations;
    }

    std::future<bool> make(Storage& input_storage, Storage& output_storage, const Product& product)
    {
        auto operations = plan(input_storage, output_storage, product);
        if (!operations) { return std::future<bool>(); }
        return std::async(std::launch::async, [operations = std::move(*operations)]() {
            for (const auto& operation : operations)
            {
                operation();
            }
            return true;
        });
    }
};

using Resource = Asset;
using Material = Product;

// example set of resources
const Resource Ores{"Ores"};
const Resource Rare_Ores{"Rare Ores"};
const Resource Energy{"Energy"};

Material metals{"Metals", Requirement{Ores, 1}};

int main()
{
    // TODO: a storage
    //   a class of objects that can make a product from resources
    //   TODO: a production line

    Storage raw_storage;
    raw_storage.resources[Ores.name] = 10;
    raw_storage.resources[Rare_Ores.name] = 5;
    raw_storage.resources[Energy.name] = 100;
    Storage finished_storage;

    Factory factory;
    auto    res = factory.plan(raw_storage, finished_storage, metals);
}