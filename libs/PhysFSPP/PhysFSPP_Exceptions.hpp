#pragma once

#include <exception>
#include <string>

namespace physfspp {
class VirtualFileSystemException : public std::exception
{
public:
    VirtualFileSystemException(const std::string& message)
      : m_message(message)
    {
    }

    const char* what() const noexcept override { return m_message.c_str(); }

private:
    std::string m_message;
};
} // namespace physfspp