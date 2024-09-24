#include "Types.hpp"
#include "Factory.hpp"
#include <deque>
#include <optional>
#include <ranges>
#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include "miniAnsi.hpp"

struct Factory2
{
};

using Resource = Asset;
using Material = Product;

// example set of resources
const Resource Ores{"Ores"};
const Resource Rare_Ores{"Rare Ores"};
const Resource Energy{"Energy"};

Material metals{"Metals", 0.5f, Requirement{Ores, 1}, Requirement{Energy, 7}};

// possible production = net_line_capactity / complexity
class ProductionLine
{
public:
    ProductionLine(Storage& input_storage, Storage& output_storage, float capacity = 1.0)
      : _input_storage{input_storage}
      , _output_storage{output_storage}
      , _capacity{capacity}
    {
    }

    using ProductAmount = std::pair<Product, Amount>;
    void tick(){};

    static float roundToNth(int fractional_parts, float value)
    {
        return std::ceil(value * fractional_parts) / fractional_parts;
    }

    void enqueue(const Product& product, Amount amount) { _queue.emplace_back(product, amount); }

    float cyclesPerFraction(const Product& product) const
    {
        return (product._fraction * product._complexity) / _capacity;
    }

    float itemsBuildablePerTick(const Product& product) const { return _capacity / product._complexity; }

    void process()
    {
        if (!_current_item)
        {
            if (_queue.empty())
            {
                spdlog::info("Queue is empty");
                return;
            }
            _current_fraction = 0;
            _current_item.emplace(_queue.front());
            _queue.pop_front();
        }
        auto&& [product, amount] = _current_item.value();
        // check if we can build another unit
        auto max_amount_from_resources = _input_storage.get_max_amount(product._requirements);
        if (max_amount_from_resources == 0) { return; }
        // how many can we build in a tick
        auto items_buildable = itemsBuildablePerTick(product);
        // how many fractions can we build in a tick
        auto max_fractions_buildable_from_capacity = items_buildable / product._fraction;
        // how many fractions can we build with the resources we have
        auto fractions_buildable_from_resources = max_amount_from_resources * (1.0f / product._fraction);
        // build fractions fitting the capacity
        auto fractions_to_build = std::min(max_fractions_buildable_from_capacity, fractions_buildable_from_resources);
        auto items_to_build_this_tick = fractions_to_build * product._fraction;
        // get resources
        for (const auto& requirement : product._requirements)
        {
            auto amount_to_take = (requirement.amount * fractions_to_build * product._fraction);
            _input_storage.take(requirement.name, amount_to_take);
        }

        // add produced fractions to output
        amount -= fractions_to_build * product._fraction;
        _current_fraction += fractions_to_build * product._fraction;
        // if we have produced a fraction of the product, put it in the output storage
        if (_current_fraction >= product._fraction)
        {
            _current_fraction = 0;

            _output_storage.put(product.name, product._fraction);
        }

        if (amount == 0) { _current_item.reset(); }
    }

    void process_old()
    {
        if (!_current_item)
        {
            if (_queue.empty()) { return; }
            _current_item.emplace(_queue.front());
            _queue.pop_front();
        }
        // check the remaining amount of product to produce
        auto&& [product, amount] = _current_item.value();
        // check how many fractions of the product could be produced
        auto max_stored = _input_storage.get_max_amount(product._requirements);
        // round up to the fractional amount
        auto max_produced = std::floor(max_stored / product._fraction);
        // adapt to complexity and capacity
        auto max_produced_by_line = std::min(max_produced, _capacity / product._complexity);
        // minimum from remaining amount and max_produced_by_line
        auto to_produce = std::min(amount, max_produced_by_line);
        // commit resources
        for (const auto& requirement : product._requirements)
        {
            _input_storage.take(requirement.name, requirement.amount * to_produce);
        }
        // remove produced fraction from the current product
        amount -= to_produce;
        // if the minimum fraction of the product was produced, put it in the output storage
        if (to_produce >= product._fraction) { _output_storage.put(product.name, to_produce); }

        if (amount == 0) { _current_item.reset(); }
    }

private:
    float                        _capacity;
    std::optional<ProductAmount> _current_item;
    float                        _current_fraction = 0;
    std::deque<ProductAmount>    _queue;
    Storage&                     _input_storage;
    Storage&                     _output_storage;
};

// template <>
// struct fmt::formatter<complex>
// {
//     template <typename ParseContext>
//     constexpr auto parse(ParseContext& ctx);
//
//     template <typename FormatContext>
//     auto format(complex const& number, FormatContext& ctx);
// };

void printStorageOneLine(const Storage& storage)
{
    // get longest key

    auto longest_key_size =
        storage.resources.empty() ? 3 : (*std::ranges::max_element(std::views::keys(storage.resources))).size() + 2;
    fmt::println("{:^{}} ", fmt::join(std::views::keys(storage.resources), "|"), longest_key_size);
    fmt::println("{:->{}}", "", storage.resources.size() * (longest_key_size + 1));
    fmt::println("{:^{}} ", fmt::join(std::views::values(storage.resources), "|"), longest_key_size);
}

int main()
{
    // TODO: a storage
    //   a class of objects that can make a product from resources
    //   TODO: a production line
    miniAnsi::setupConsole();
    Storage raw_storage;
    raw_storage.resources[Ores.name] = 2;
    raw_storage.resources[Rare_Ores.name] = 5;
    raw_storage.resources[Energy.name] = 100;
    Storage finished_storage;

    printStorageOneLine(raw_storage);
    if (0)
    {
        Factory factory;
        auto    res = factory.make(raw_storage, finished_storage, metals);
        res.get();
    }

    ProductionLine line(raw_storage, finished_storage, 0.125);
    line.enqueue(metals, 2);

    while (true)
    {
        miniAnsi::clearScreen();
        miniAnsi::moveCursor(0, 0);
        line.process();
        printStorageOneLine(raw_storage);
        printStorageOneLine(finished_storage);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}