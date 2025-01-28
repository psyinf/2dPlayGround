#pragma once

#include "FractionalAmount.hpp"
#include <fmt/format.h>


template <typename DigitsType, DigitsType NumDigits>
struct fmt::formatter<FractionalAmount<DigitsType, NumDigits>> : formatter<float>
{
    auto format(FAmount const& c, format_context& ctx) const { return formatter<float>::format(c, ctx); }
};