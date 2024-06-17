#pragma once

namespace pg::game {

class ResourceNotFoundException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};
} // namespace pg::game