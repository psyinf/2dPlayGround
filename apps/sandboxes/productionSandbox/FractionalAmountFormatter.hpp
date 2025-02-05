#pragma once

#include "FractionalAmount.hpp"
#include "Types.hpp"
#include <format>

// std formatter

template <typename DigitsType, DigitsType NumDigits>
struct std::formatter<FractionalAmount<DigitsType, NumDigits>> : std::formatter<float>
{
    auto format(FractionalAmount<DigitsType, NumDigits> a, format_context& ctx) const
    {
        return formatter<float>::format(a, ctx);
    }
};

// std formatter
// template <>
// struct std::formatter<Person> : std::formatter<std::string>
// {
//     auto format(const Person& p, std::format_context& ctx)
//     {
//         return std::format_to(ctx.out(), "Name: {}, Age: {}", p.name, p.age);
//     }
// };