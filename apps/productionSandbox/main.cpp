#include "Types.hpp"
#include "Factory.hpp"
#include <deque>
#include <optional>

struct Factory2
{
};

using Resource = Asset;
using Material = Product;

// example set of resources
const Resource Ores{"Ores"};
const Resource Rare_Ores{"Rare Ores"};
const Resource Energy{"Energy"};

Material metals{"Metals", Requirement{Ores, 1}};

// possible production = net_line_capactity / complexity
class ProductionLine
{
    using ProductAmount = std::pair<Product, Amount>;
    void tick(){};

    void enqueue(const Product& product, Amount amount) { queue.emplace_back(product, amount); }

    void process()
    {
        if (!current_product)
        {
            if (queue.empty()) { return; }
            current_product.emplace(queue.front());
            queue.pop_front();
        }
        // check the remaining amount of product to produce
        auto&& [product, amount] = current_product.value();
        // check how many fractions of the product could be produced
        auto max_stored = input_storage.get_max_amount(product._requirements);
        // round up to the fractional amount
        auto max_produced = std::floor(max_stored / product._fraction);
        // adapt to complexity and capacity
        auto max_produced_by_line = std::min(max_produced, capacity / product._complexity);
        // minimum from remaining amount and max_produced_by_line
        auto to_produce = std::min(amount, max_produced_by_line);
        // commit resources
        for (const auto& requirement : product._requirements)
        {
            input_storage.take(requirement.name, requirement.amount * to_produce);
        }
        // remove produced fraction from the current product
        amount -= to_produce;
        // if the product is fully produced, remove it from the current product and put it in the output storage

        if (amount == 0)
        {
            output_storage.put(product.name, to_produce);
            current_product.reset();
        }
    }

    float                        capacity;
    std::optional<ProductAmount> current_product;
    std::deque<ProductAmount>    queue;
    Storage&                     input_storage;
    Storage&                     output_storage;
};

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
    auto    res = factory.make(raw_storage, finished_storage, metals);
    res.get();
}