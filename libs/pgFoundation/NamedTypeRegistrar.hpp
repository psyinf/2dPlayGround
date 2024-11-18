#pragma once

#include <pgFoundation/NamedTypeRegistry.hpp>

namespace pgf {
template <size_t N>
struct StringLiteral
{
    constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N - 1, value); }

    char value[N - 1];

    operator std::string() const { return std::string(value, N - 1); }

    operator std::string_view() const { return std::string_view(value, N - 1); }
};

template <typename T>
constexpr void registerEvent(const std::string name)
{
    pgf::NamedTypeRegistry::registerType<T>(std::string{name});
}

template <typename T, StringLiteral name>
struct TypeRegistrar
{
    constexpr TypeRegistrar() { registerEvent<T>(name); }
};
} // namespace pgf