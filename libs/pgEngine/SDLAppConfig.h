#pragma once
#include <string>
#include "SDLVec.h"
namespace pg::config {

struct WindowConfig
{
    pg::Vec2Window  offset;
    pg::Vec2Window  size;
    std::string windowName;
};

} // namespace pg::config