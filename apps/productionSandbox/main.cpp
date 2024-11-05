#include "Types.hpp"
#include "Factory.hpp"
#include <deque>
#include <optional>
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


using Resource = Asset;
using Material = Product;

// example set of resources
const Resource Ores{"Ores"};
const Resource Rare_Ores{"Rare Ores"};
const Resource Energy{"Energy"};

Material metals{"Metals", 0.5f, Requirement{Ores, 1}, Requirement{Rare_Ores, 0.005}, Requirement{Energy, 7}};

// possible production = net_line_capactity / complexity
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
    FAmount a{1.0f};
    FAmount b;
    b = 1.0f;
    FAmount c = 3.0f;
    // TODO: a storage00
    //   a class of objects that can make a product from resources
    //   TODO: a production line
    pg::foundation::console::setupConsole();
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

    ProductionLine line(raw_storage, finished_storage, 0.1);
    line.enqueue(metals, 2);

    while (true)
    {
        pg::foundation::console::clearScreen();
        pg::foundation::console::moveCursor(0, 0);
        std::cout << magic_enum::enum_name( line.process()) << std::endl;
        printStorageOneLine(raw_storage);
        printStorageOneLine(finished_storage);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}