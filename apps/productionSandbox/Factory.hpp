#pragma once
#include "Types.hpp"
#include "Storage.hpp"
#include <functional>
#include <optional>
#include <vector>
#include <future>

struct Factory
{
    using Task = std::function<void()>;
    using TaskList = std::vector<Task>;
    using Order = std::optional<TaskList>;
    Factory() = default;

    // plan how to produce a product, by taking resources from input_storage and putting the product in output_storage

    Order plan(Storage& input_storage, Storage& output_storage, const Product& product)
    {
        // list of operations to reduce the amount of resources in storage
        TaskList operations;
        // TODO: we need to rollback resources if we can't produce the product
        for (const auto& requirement : product._requirements)
        {
            if (!input_storage.has_amount(requirement.name, requirement.amount)) { return std::nullopt; }
            operations.emplace_back(
                [&input_storage, &requirement]() { input_storage.take(requirement.name, requirement.amount); });
        }
        operations.emplace_back(
            [&output_storage, &product]() mutable { output_storage.put(product.name, product._fraction); });

        return operations;
    }

    std::future<bool> execute(const Order& order)
    {
        if (!order) { return std::future<bool>(); }

        auto operations = *order;
        return std::async(std::launch::async, [operations]() {
            for (auto& operation : operations)
            {
                operation();
            }
            return true;
        });
    }

    std::future<bool> make(Storage& input_storage, Storage& output_storage, const Product& product)

    {
        auto operations = plan(input_storage, output_storage, product);

        if (!operations) { return std::future<bool>(); }

        return execute(operations);
    }
};
