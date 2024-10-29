#include "Types.hpp"
#include "Factory.hpp"
#include <deque>
#include <optional>
#include <ranges>
#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include "miniAnsi.hpp"
#include "FractionalAmount.hpp"

#include <magic_enum.hpp>
struct Factory2
{
};

using Resource = Asset;
using Material = Product;

// example set of resources
const Resource Ores{"Ores"};
const Resource Rare_Ores{"Rare Ores"};
const Resource Energy{"Energy"};

Material metals{"Metals", 0.5f, Requirement{Ores, 1.f}, Requirement{Rare_Ores, 0.005f}, Requirement{Energy, 7.f}};

// possible production = net_line_capactity / complexity
class ProductionLine
{
public:
    enum class State
    {
        EMPTY,                 //< currently no items in queue
        WAITING_FOR_RESOURCES, //< currently produced item waits for resources
        FINISHED,              //< item production finished
        RUNNING,               //< currently producing
    };

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

    auto itemsBuildablePerTick(const Product& product) const { return _capacity / product._complexity; }

    State process()
    {
        if (!_current_item)
        {
            if (_queue.empty()) { return State::EMPTY; }
            _current_fraction = 0;
            _current_item.emplace(_queue.front());
            _queue.pop_front();
        }
        auto&& [product, amount] = _current_item.value();
        // check if we can build another unit
        auto max_amount_from_resources = _input_storage.get_max_amount(product._requirements);
        if (max_amount_from_resources == 0.0f) { return State::WAITING_FOR_RESOURCES; }

        // how many can we build in a tick
        auto items_buildable = itemsBuildablePerTick(product);
        // how many fractions can we build in a tick
        auto max_fractions_buildable_from_capacity = items_buildable / product._fraction;
        // how many fractions can we build with the resources we have
        auto fractions_buildable_from_resources = max_amount_from_resources * FAmount(1.0f / float(product._fraction));
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

        if (amount == 0.0f)
        {
            _current_item.reset();
            return State::FINISHED;
        }
        return State::RUNNING;
    }

    // TODO: a type that represents fractions as powers of 2
    // e.g 1.0, 1.25, 1.75, 0.125, 0.0625, 0.03125

private:
    Amount                       _capacity;
    std::optional<ProductAmount> _current_item;
    Amount                       _current_fraction = 0.0f;
    std::deque<ProductAmount>    _queue;
    State                        _state{State::EMPTY};
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
    miniAnsi::setupConsole();
    Storage raw_storage;
    raw_storage.resources[Ores.name] = 2;
    raw_storage.resources[Rare_Ores.name] = 5;
    raw_storage.resources[Energy.name] = 100;
    Storage finished_storage;

    printStorageOneLine(raw_storage);

    ProductionLine line(raw_storage, finished_storage, 0.1);
    line.enqueue(metals, 2);

    while (true)
    {
        miniAnsi::clearScreen();
        miniAnsi::moveCursor(0, 0);
        std::cout << magic_enum::string( line.process()) << std::endl;
        printStorageOneLine(raw_storage);
        printStorageOneLine(finished_storage);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}