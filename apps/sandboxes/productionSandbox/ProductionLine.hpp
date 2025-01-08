#pragma once
#include "Storage.hpp"
#include "Types.hpp"

#include <optional>
#include <spdlog/spdlog.h>
#include <deque>

static constexpr bool splitQueue = true;

/**
 * @brief A production line creates products from resources
 * It has an input storage and an output storage and a production capacity that determines the amount of complexity that
 * can be produced in a tick. The production line can enqueue products to be produced and process them in a tick.
 */
class ProductionLine
{
public:
    enum class State
    {
        EMPTY_QUEUE,           //< currently no items in queue
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

    void enqueue(const Product& product, Amount amount)
    {
        if (!splitQueue)
        {
            _queue.emplace_back(product, amount);
            return;
        }
        else
        {
            int fractions = amount / product._fraction;
            for (int i = 0; i < fractions; ++i)
            {
                _queue.emplace_back(product, product._fraction);
            }
        }
    }

    float cyclesPerFraction(const Product& product) const
    {
        return (product._fraction * product._complexity) / _capacity;
    }

    auto itemsBuildablePerTick(const Product& product) const { return _capacity / product._complexity; }

    size_t queueSize() const { return _queue.size(); }

    bool isEmpty() const { return _queue.empty(); }

    Fraction currentFraction() const { return _current_fraction; }

    // will return the current product being produced. If the product is finished, it will return an empty optional as
    // the product is already in the output storage
    auto currentProduct() const { return _current_item; }

    Fraction maxAmountFromResources(const Product& product) const
    {
        return _input_storage.get_max_amount(product._requirements);
    }

    State process()
    {
        if (!_current_item)
        {
            if (_queue.empty()) { return State::EMPTY_QUEUE; }
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
            _current_fraction = 0.0;
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
    State                        _state{State::EMPTY_QUEUE};
    Storage&                     _input_storage;
    Storage&                     _output_storage;
};