#pragma once
#include <string>

#include <pgFoundation/NamedTypeRegistrar.hpp>

namespace galaxy::events {

struct MenuButtonPressed : public pgf::TypeRegistrar<MenuButtonPressed, "MenuButtonPressed">
{
    std::string menuName;
    std::string buttonName;
};

} // namespace galaxy::events