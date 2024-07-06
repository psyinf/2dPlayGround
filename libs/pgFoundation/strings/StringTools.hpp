#pragma once

#include <string>
#include <vector>

namespace foundation::strings {

bool matches(const std::string& s, const std::string& pattern);

bool matchesWildCard(const std::string& s, const std::string& wildcardPattern, bool caseSensitive = true);

std::vector<std::string_view> tokenize(std::string_view str, const std::string_view delims = " ");

} // namespace foundation::strings