#pragma once
#include <string>
namespace galaxy::events {

struct MenuButtonPressed
{
    std::string menuName;
    std::string buttonName;
};

} // namespace galaxy::events