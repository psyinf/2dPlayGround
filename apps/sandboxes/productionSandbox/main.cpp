#include "Types.hpp"
#include "Factory.hpp"
#include <deque>
#include <optional>
#include <algorithm>
#include <ranges>
#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <magic_enum.hpp>
#include <pgf/console/miniAnsi.hpp>
#include "FractionalAmount.hpp"
#include "FractionalAmoundFormatter.hpp"
#include "Storage.hpp"
#include "ProductionLine.hpp"
#include <iostream>

using Resource = Asset;
using Material = Product;

void printStorageOneLine(const Storage& storage)
{
    // get longest key

    auto longest_key_size =
        storage.resources.empty() ? 3 : (*std::ranges::max_element(std::views::keys(storage.resources))).size() + 2;
    fmt::println("{:^{}} ", fmt::join(std::views::keys(storage.resources), "|"), longest_key_size);
    fmt::println("{:->{}}", "", storage.resources.size() * (longest_key_size + 1));
    fmt::println("{:^{}} ", fmt::join(std::views::values(storage.resources), "|"), longest_key_size);
}

void printProductionLineState(const ProductionLine& productionLine)
{
    auto [product, amount] = productionLine.currentProduct().value_or(ProductionLine::ProductAmount{"N/A", 0.0f});
    fmt::println("Current Product: {} ({})", product.name, productionLine.currentFraction());
}

void test1()
{
    const auto Hydrogen = Resource{"Hydrogen"};
    const auto Oxygen = Resource{"Oxygen"};
    const auto Water = Product{"Water", Requirement{Hydrogen, 2}, Requirement{Oxygen, 1}};

    Storage raw_storage;
    raw_storage.resources[Hydrogen.name] = 20;
    raw_storage.resources[Oxygen.name] = 10;

    Storage finished_storage;

    ProductionLine line(raw_storage, finished_storage, 0.1f);

    line.enqueue(Water, 10);
    auto state = ProductionLine::State::RUNNING;
    while (state == ProductionLine::State::RUNNING || state == ProductionLine::State::FINISHED)
    {
        state = line.process();
        pg::foundation::console::clearScreen();
        pg::foundation::console::moveCursor(0, 0);
        printProductionLineState(line);
        printStorageOneLine(raw_storage);
        printStorageOneLine(finished_storage);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void test2()
{
    // example set of resources
    const Resource Ores{"Ores"};
    const Resource Rare_Ores{"Rare Ores"};
    const Resource Energy{"Energy"};

    Material metals{"Metals", 0.5f, Requirement{Ores, 1}, Requirement{Rare_Ores, 0.005f}, Requirement{Energy, 7}};

    Storage raw_storage;
    raw_storage.resources[Ores.name] = 2;
    raw_storage.resources[Rare_Ores.name] = 5;
    raw_storage.resources[Energy.name] = 100;
    Storage finished_storage;

    printStorageOneLine(raw_storage);
    printStorageOneLine(finished_storage);
    if (0)
    {
        Factory factory;
        auto    res = factory.make(raw_storage, finished_storage, metals);
        res.get();
    }

    ProductionLine line(raw_storage, finished_storage, 0.1f);
    line.enqueue(metals, 2);

    while (true)
    {
        pg::foundation::console::clearScreen();
        pg::foundation::console::moveCursor(0, 0);
        std::cout << magic_enum::enum_name(line.process()) << std::endl;
        printStorageOneLine(raw_storage);
        printStorageOneLine(finished_storage);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    pg::foundation::console::setupConsole();
    test1();
}